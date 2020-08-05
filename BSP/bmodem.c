#include "bmodem.h"
#include "bsp.h"
#include "mini-printf.h"
#include "wearLevelFlash.h"

#define PACKET_SIZE             (128)
/* start of 128-byte data packet */
#define SOH                     (0x01)
/* end of transmission */
#define EOT                     (0x04)
/* acknowledge */
#define ACK                     (0x06)
/* negative acknowledge */
#define NAK                     (0x15)
//cancel
#define CAN (0x18)

#define NAK_TIMEOUT             (0x50000)
#define MAX_RETRY               (5)

#define ERR_INVALID -10
#define ERR_TIMEOUT -11

static const char* errMessage="none";

const char* bmodem_getErrMessage(void){
    return errMessage;
}

static uint32_t _getSectorByAddr(uint32_t addr){
    return getSectorByAddr(addr);
}
static bool _eraseSector(uint32_t sector){
    eraseSector(sector);
    return true;
}
static bool _writeFlashU32(uint32_t addr,uint32_t* data,uint32_t count){
    writeFlashU32(addr,data,count);
    return true;
}

static uint16_t updateCRC16(uint16_t crcIn, uint8_t byte)
{
  uint32_t crc = crcIn;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}

static uint16_t calcCRC16(const uint8_t* data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd){
      crc = updateCRC16(crc, *data++);
  }
 
  crc = updateCRC16(crc, 0);
  crc = updateCRC16(crc, 0);

  return crc&0xffffu;
}

/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *        -1: Timeout
  */
static int32_t rx(){
    int r;
    uint32_t timeout=NAK_TIMEOUT;
    while (timeout-- > 0)
    {
        r=getc();
        if(r>=0){
            return r;
        }
    }
    return -1;
}
static void tx(uint8_t c){
    putc(c);
}

static uint8_t rxPacketBuffer[PACKET_SIZE];
static int32_t rxPacketSize=0;
static uint8_t rxPacketId=0;

/**
  * @brief  Receive a packet from sender
  * @retval 0: normally return
  *        -1: timeout or packet error
  *        -2: user canceled
  */
static int32_t rxPacket()
{
    int32_t packetType=0;
    {
        rxPacketId=0xFF;
        packetType=rx();
        if (packetType < 0){
            return ERR_TIMEOUT;
        }
        switch (packetType){
            case SOH:
              rxPacketSize = PACKET_SIZE;
              break;
            case EOT:
              return packetType;
            default:
              return ERR_INVALID;
        }
    }
    {
        int32_t pid=rx();
        if(pid < 0){
            return ERR_TIMEOUT;
        }
        int32_t ipid = rx();
        if(ipid < 0){
            return ERR_TIMEOUT;
        }
        ipid|=0xFFFFFF00;
        if(pid != ~ipid){
            return ERR_INVALID;
        }
        rxPacketId=pid;
    }
    
    for (int32_t i = 0; i < rxPacketSize; i ++){
        int32_t c=rx();
        if(c < 0){
            return ERR_TIMEOUT;
        }
        rxPacketBuffer[i]=c;
    }
    {
        int32_t crc1=rx();
        if(crc1 < 0){
            return ERR_TIMEOUT;
        }
        int32_t crc2=rx();
        if(crc2 < 0){
            return ERR_TIMEOUT;
        }
        uint16_t crc16=calcCRC16(rxPacketBuffer,rxPacketSize);
        if( (crc16>>8) == crc1 && (crc16&0xFF) == crc2){
            return packetType;
        }
        return ERR_INVALID;
    }
}

static void waitTimeout(){
    while(1){
        int32_t ret=rx();
        if(ret<0){
            break;
        }
    }
}

static uint32_t appHead[2]={0,0};
static bool headCaptured=false;
static uint32_t appAddr=0;

int bmodem_receive (uint32_t startAddr,uint32_t endAddr){
    int32_t  filesize=0;
    uint32_t currentAddr=startAddr;
    uint32_t maxSize=endAddr-startAddr;
    uint8_t  currentPacketId=0;
    uint32_t lastSector=0;
    uint32_t retryCount=0;

    headCaptured=false;
    appAddr=startAddr;
    while(1){
        tx(NAK);
        int32_t ret=rxPacket();
        if( (ret == SOH) && (rxPacketId==0) ){
            filesize=((uint32_t*)rxPacketBuffer)[0];
            if(filesize > maxSize){
                errMessage="file size exceed FLASH_SIZE";
                goto FAILED;
            }
            currentPacketId++;
            tx(ACK);
            retryCount=0;
            break;
        }
        waitTimeout();
        retryCount++;
        if(retryCount > MAX_RETRY){
            return 0;
        }
    }
    while(1){
        int32_t ret=rxPacket();
        if( (ret == SOH) && (rxPacketId==currentPacketId) ){
            if(currentAddr + rxPacketSize > endAddr){
                errMessage="write addr exceeded EndAddr";
                goto FAILED;
            }
            uint32_t sector=_getSectorByAddr(currentAddr);
            if(sector != lastSector){
                if(!_eraseSector(sector)){
                    errMessage="erase sector failed";
                    goto FAILED;
                }
                lastSector=sector;
            }
            if(headCaptured==false){
                uint32_t* lp32=(uint32_t*)rxPacketBuffer;
                appHead[0]=lp32[0];
                appHead[1]=lp32[1];
                headCaptured=true;
                lp32[0]=0xFFFFFFFF;
                lp32[1]=0xFFFFFFFF;
            }
            if( !_writeFlashU32(currentAddr, (uint32_t*)rxPacketBuffer, rxPacketSize/4) ){
                errMessage="write flash failed";
                goto FAILED;
            }
            currentAddr += rxPacketSize;
            currentPacketId++;
            retryCount=0;
            tx(ACK);
            continue;
        }else if(ret==EOT){
            for(volatile int32_t i=0;i<100000;i++){
                __nop();__nop();__nop();__nop();__nop();
            }
            tx(ACK);
            return 1;
        }else{
            //printf("unknow ret:%d, pkgid=%d,curId=%d\r\n",ret,rxPacketId,currentPacketId);
        }
        waitTimeout();
        retryCount++;
        if(retryCount > MAX_RETRY){
            errMessage="receive data packet failed";
            goto FAILED;
        }
        tx(NAK);
    }
FAILED:
    tx(CAN);
    return -1;
}
void bmodem_finish (){
    _writeFlashU32(appAddr,appHead,2);
}
