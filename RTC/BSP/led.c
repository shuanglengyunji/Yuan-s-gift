#include "led.h"

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	//使能GPIOC的外设时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			//选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//设置引脚模式为推免输出模式				 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚速度为50MHZ
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
