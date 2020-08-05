#pragma once

#include "stm32f10x.h"
#include <stdbool.h>


#ifdef __cplusplus
extern "C"{
#endif

#define DIR_OUTPUT true
#define DIR_INPUT false
#define LOGIC_NORMAL false
#define LOGIC_INVERTED true
#define DEFAULT_SET true
#define DEFAULT_RESET false

typedef struct{
	GPIO_TypeDef *base;         //GPIOx
	uint16_t      pin;          //GPIO_Pin_x
    bool          direction;    //true:Output , false:input
    bool          inverted;     //false:high=true,low=false      true:high=false,low=true
	bool          initState;    //default value
}gpio_t;

void gpio_init(gpio_t *gpio); //push-pull output / float-input
void gpio_init_custom(gpio_t *gpio,GPIOMode_TypeDef mode,GPIOSpeed_TypeDef clkSpeed);

static void gpio_set(gpio_t *gpio,bool state){
	if(state^gpio->inverted){
		gpio->base->BSRR|=gpio->pin;
	}else{
		gpio->base->BRR|=gpio->pin;
	}
}

static bool gpio_get(gpio_t *gpio){
	return gpio->inverted^(((gpio->base->IDR)&(gpio->pin))?true:false);
}

static bool gpio_getOutput(gpio_t *gpio){
	return gpio->inverted^(((gpio->base->ODR)&(gpio->pin))?true:false);
}

#ifdef __cplusplus
}
#endif

