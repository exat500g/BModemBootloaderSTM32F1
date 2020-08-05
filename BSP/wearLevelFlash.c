#include "wearLevelFlash.h"

static bool isValidSector(int32_t sector){
    if(sizeof(SectorData)!=1024){
        return false;
    }
    if(sector>=START_SECTOR && sector<=END_SECTOR){
        return true;
    }
    return false;
}

uint8_t getSectorByAddr(uint32_t addr){
    uint32_t sector=(addr - 0x08000000)/SECTOR_SIZE;
    if(isValidSector(sector)){
        return sector;
    }
    return 0xFF;
}

uint32_t getSectorAddr(uint8_t sector){
    if(isValidSector(sector)){
        return (FLASH_BASE + sector*sizeof(SectorData));
    }
    return 0xFFFFFFFF;
}

int32_t getValidSector(){
    for(int32_t i=START_SECTOR;i<END_SECTOR;i++){
        SectorData* p=getSectorData(i);
        if(p->head[0]==0x55 && p->head[1]==0xAA){
            uint16_t checksum=0;
            for(uint32_t i=0; i<SECTOR_DATA_NUM; i++){
                checksum+=p->data[i];
            }
            if(checksum==p->checksum){
                return i;
            }
        }
    }
    return -1;
}
void writeNextSector(SectorData *data){
    int currentSector=getValidSector();
    int nextSector=START_SECTOR;
    if(isValidSector(currentSector)){
        SectorData*p=getSectorData(currentSector);
        for(int i=0;i<SECTOR_DATA_NUM;i++){
            if(p->data[i]!=data->data[i]){
                goto LabelConfirmWrite;
            }
        }
        return;
    }
LabelConfirmWrite:
    if(isValidSector(currentSector)){
        nextSector=currentSector+1;
        if(nextSector>END_SECTOR){
            nextSector=START_SECTOR;
        }
    }
    eraseSector(nextSector);
    writeSectorData(nextSector,data);
    eraseSector(currentSector);
}


SectorData* getSectorData(uint8_t sector){
    SectorData *p=(SectorData*)getSectorAddr(sector);
    return p;
}

void eraseSector(uint8_t sector){
    if(isValidSector(sector)){
        FLASH_Unlock();
        FLASH_ErasePage(getSectorAddr(sector));
        FLASH_Lock();
    }
}

void writeSectorData(uint8_t sector,SectorData *data){
    if(isValidSector(sector)){
        FLASH_Unlock();
        uint16_t *pu16=(uint16_t*)data;
        uint32_t addr=getSectorAddr(sector);
        data->head[0]=0x55;
        data->head[1]=0xAA;
        data->checksum=0;
        for(uint32_t i=0; i<SECTOR_DATA_NUM; i++){
            data->checksum+=data->data[i];
        }
        for(uint32_t i=0; i<sizeof(SectorData)/sizeof(pu16[0]); i++){
            FLASH_ProgramHalfWord(addr+i*sizeof(pu16[0]),pu16[i]);
        }
        FLASH_Lock();
    }
}
void writeFlashU32(uint32_t addr,uint32_t* data,uint32_t count){
    FLASH_Unlock();
    for(uint32_t i=0; i<count; i++){
        FLASH_ProgramWord(addr+i*sizeof(uint32_t),data[i]);
    }
    FLASH_Lock();
}
