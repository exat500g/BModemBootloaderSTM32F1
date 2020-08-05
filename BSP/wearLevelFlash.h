#ifndef __WEAR_LEVEL_FLASH_H
#define __WEAR_LEVEL_FLASH_H

#include "stm32f10x.h"
#include <stdbool.h>

#if defined(STM32F10X_MD)

#define SECTOR_SIZE 1024
#define SECTOR_DATA_NUM 510
#define START_SECTOR 5
#define END_SECTOR   40

typedef struct{
    uint8_t  head[2];    //0x55 0xAA
    uint16_t checksum;   //sum(data)
    uint16_t data[SECTOR_DATA_NUM];
}SectorData;

#endif

uint8_t getSectorByAddr(uint32_t addr);
uint32_t getSectorAddr(uint8_t sector);
int32_t getValidSector(void);  //return -1 when err
void    writeNextSector(SectorData *data);

SectorData* getSectorData(uint8_t sector);
void        eraseSector(uint8_t sector);
void        writeSectorData(uint8_t sector,SectorData *data);
void        writeFlashU32(uint32_t addr,uint32_t* data,uint32_t count);



#endif
