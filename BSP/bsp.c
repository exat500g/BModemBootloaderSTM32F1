#define  BSP_MODULE

#include "bsp.h"


#if 0
#define UART               USART3
#define UART_RCC_ENABLE()  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE)
#define GPIO_RCC_ENABLE()  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define UART_GPIO		   GPIOB
#define UART_GPIO_TX	   GPIO_Pin_10
#define UART_TX_PIN_SOURCE GPIO_PinSource10
#define UART_GPIO_RX	   GPIO_Pin_11
#define UART_RX_PIN_SOURCE GPIO_PinSource11
#define GPIO_AF_USART      GPIO_AF_USART3
#endif

#if 1
#define UART                USART2
#define UART_RCC_ENABLE(EN) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,EN)
#define UART_RCC_RESET(EN)  RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,EN)
#define GPIO_RCC_ENABLE(EN) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, EN)
#define GPIO_RCC_RESET(EN)  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, EN)
#define UART_GPIO		   GPIOA
#define UART_GPIO_TX	   GPIO_Pin_2
#define UART_TX_PIN_SOURCE GPIO_PinSource2
#define UART_GPIO_RX	   GPIO_Pin_3
#define UART_RX_PIN_SOURCE GPIO_PinSource3
#define GPIO_AF_USART      GPIO_AF_USART2
#endif

#if 0
#define UART                USART1
#define UART_RCC_ENABLE(EN) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,EN)
#define UART_RCC_RESET(EN)  RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,EN)
#define GPIO_RCC_ENABLE(EN) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, EN)
#define GPIO_RCC_RESET(EN)  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, EN)
#define UART_GPIO		   GPIOB
#define UART_GPIO_TX	   GPIO_Pin_6
#define UART_TX_PIN_SOURCE GPIO_PinSource6
#define UART_GPIO_RX	   GPIO_Pin_7
#define UART_RX_PIN_SOURCE GPIO_PinSource7
#define GPIO_AF_USART      GPIO_AF_USART1
#endif

#if 0
#define UART                USART1
#define UART_RCC_ENABLE(EN) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,EN)
#define UART_RCC_RESET(EN)  RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,EN)
#define GPIO_RCC_ENABLE(EN) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, EN)
#define GPIO_RCC_RESET(EN)  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, EN)
#define UART_GPIO		   GPIOA
#define UART_GPIO_TX	   GPIO_Pin_9
#define UART_TX_PIN_SOURCE GPIO_PinSource9
#define UART_GPIO_RX	   GPIO_Pin_10
#define UART_RX_PIN_SOURCE GPIO_PinSource10
#define GPIO_AF_USART      GPIO_AF_USART1
#endif

USART_TypeDef* USART=UART;

static void USART_Configuration(void){
	UART_RCC_ENABLE(ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = UART_GPIO_TX;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(UART_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = UART_GPIO_RX;
	GPIO_Init(UART_GPIO, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //9b when party is set
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(UART,&USART_InitStructure);
	USART_Cmd(UART,ENABLE);
}

static void RCC_Configuration(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

gpio_t led1={GPIOA,GPIO_Pin_1, DIR_OUTPUT, LOGIC_NORMAL, DEFAULT_RESET};
gpio_t RS485_RE={GPIOA,GPIO_Pin_4, DIR_OUTPUT, LOGIC_NORMAL, DEFAULT_RESET};
gpio_t RS485_DE={GPIOA,GPIO_Pin_5, DIR_OUTPUT, LOGIC_NORMAL, DEFAULT_RESET};

static void GPIO_Configuration(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpio_init(&led1);
    gpio_init(&RS485_RE);
    gpio_init(&RS485_DE);
}
int rc=-1;
int tc=12;
int test=0;
int rxCount=0;
void  BSP_Init (void){
	RCC_Configuration();
	GPIO_Configuration();
    USART_Configuration();
    /*while(1){
        rc=getc();
        if(rc>=0){
            rxCount++;
        }
        if(test==1){
            test=0;
            putc(tc);
        }
    }*/
}
void  BSP_DeInit (void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
    UART_RCC_ENABLE(DISABLE);
}

int putc(int c) {
    while (!(USART->SR & USART_SR_TXE)){__ASM("nop");}
    gpio_set(&RS485_RE,true);
    gpio_set(&RS485_DE,true);
    USART->CR1 &= ~USART_CR1_RE;
    volatile uint16_t sr = USART->SR;  //read SR then write DR to clear SR_TC flag
    USART->DR = c;
    while (!(USART->SR & USART_SR_TC)){__ASM("nop");}
    gpio_set(&RS485_RE,false);
    gpio_set(&RS485_DE,false);
    USART->CR1 |= USART_CR1_RE;
	return 1;
}

int puts(char *s,unsigned int len){
    gpio_set(&RS485_RE,true);
    gpio_set(&RS485_DE,true);
    USART->CR1 &= ~USART_CR1_RE;
    volatile uint16_t sr = USART->SR;  //read SR then write DR to clear SR_TC flag
    for(unsigned int i=0;i<len;i++){
        while (!(USART->SR & USART_SR_TXE)){__ASM("nop");}
        USART->DR = s[i];
    }
    while (!(USART->SR & USART_SR_TC)){__ASM("nop");}
    gpio_set(&RS485_RE,false);
    gpio_set(&RS485_DE,false);
    USART->CR1 |= USART_CR1_RE;
    return len;
}

int getc(void){
    if(USART->SR & USART_SR_RXNE){
        return USART->DR&0x00FF;
    }
    return -1;
}
void delay(int32_t t){
    for(volatile int32_t i=0;i<t;i++){
        __ASM("nop");
    }
}
