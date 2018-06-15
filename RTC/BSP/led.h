#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define	LED0_ON  GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define	LED0_OFF GPIO_SetBits(GPIOC, GPIO_Pin_13)

#define	LED1_ON  GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define	LED1_OFF GPIO_SetBits(GPIOB, GPIO_Pin_12)

void LED_GPIO_Config(void);	

#endif
