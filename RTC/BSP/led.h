#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define		LED0_ON   GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define		LED0_OFF  GPIO_ResetBits(GPIOC, GPIO_Pin_13)

void LED_GPIO_Config(void);	

#endif
