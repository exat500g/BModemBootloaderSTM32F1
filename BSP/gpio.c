#include "gpio.h"

void gpio_init(gpio_t* gpio){
    GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = gpio->pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = gpio->direction?GPIO_Mode_Out_PP:GPIO_Mode_IN_FLOATING;
	GPIO_Init(gpio->base, &GPIO_InitStructure);
	gpio_set(gpio,gpio->initState);
}

void gpio_init_custom(gpio_t *gpio,GPIOMode_TypeDef mode,GPIOSpeed_TypeDef clkSpeed){
    GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = gpio->pin;
	GPIO_InitStructure.GPIO_Speed = clkSpeed;
	GPIO_InitStructure.GPIO_Mode  = mode;
	GPIO_Init(gpio->base, &GPIO_InitStructure);
	gpio_set(gpio,gpio->initState);
}

