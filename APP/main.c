#include "wearLevelFlash.h"
#include "bmodem.h"
#include "bsp.h"
#include "mini-printf.h"
#include <stm32f10x_iwdg.h>
#include <stm32f10x_dbgmcu.h>

typedef  void (*pFunction)(void);

//1024*5 = 0x1400

#define APP_START_ADDRESS (0x08000000 + START_SECTOR*SECTOR_SIZE)
#define APP_END_ADDRESS (0x08000000 + END_SECTOR*SECTOR_SIZE)

int main(void)
{
    //SystemCoreClockUpdate();
    //DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP,ENABLE);
    __IO uint32_t* appVectorTable = (__IO uint32_t*)APP_START_ADDRESS;
    uint32_t appEntry;
    uint32_t spAddress;
    BSP_Init();
    while(1){
        while(1){
            printf("\r\nBModem-bootloader started...\r\n");
            int32_t errCode = 0;
            delay(100000);
            errCode = bmodem_receive(APP_START_ADDRESS,APP_END_ADDRESS);
            delay(100000);
            if (errCode == 0){  //nothing to do
                break;
            }else if(errCode==1){  //SUCC
                printf("\r\nUpload Successfully!\r\n");
                bmodem_finish();
                break;
            }else{  //FAILED
                printf("err:%s\r\n",bmodem_getErrMessage());
            }
            delay(100000);
        }
        FLASH_Lock();

        spAddress  = appVectorTable[0];
        appEntry = appVectorTable[1];
        if(appEntry<=APP_END_ADDRESS && appEntry>=APP_START_ADDRESS && spAddress>=SRAM_BASE && spAddress<=(SRAM_BASE+0x20000) ){
            pFunction application = (pFunction) appEntry;
            printf("Starting application...\r\n");
            delay(100000);
            BSP_DeInit();
			
            IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
            IWDG_SetPrescaler(IWDG_Prescaler_256);	//6.4ms/load
            IWDG_SetReload(469);	//469*6.4ms=3s
            IWDG_ReloadCounter();
            IWDG_Enable();
    
            __disable_irq();
            NVIC_SetVectorTable(APP_START_ADDRESS, 0);
            __set_MSP(spAddress);
			
            application();
        }else{
            printf("Invalid application\r\n");
            delay(10000000);
        }
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
  printf("\r\nWrong parameters value: file %s on line %d\r\n",file,line);
  while (1){;}
}
