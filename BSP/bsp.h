#ifndef _H_BSP_
#define _H_BSP_

#include "stm32f10x.h"
#include "gpio.h"

void BSP_Init(void);
void BSP_DeInit(void);

extern USART_TypeDef* USART;
extern gpio_t led1;

int putc(int c);
int puts(char *s,unsigned int len);
int getc(void);

void delay(int32_t t);

#endif

