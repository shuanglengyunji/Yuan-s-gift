#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "RTC_Time.h" 
#include <stdio.h>

uint32_t standby_counter = 0;

void Delay (uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

void OSC_Shut_Down(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD ,ENABLE);	//使能GPIOC的外设时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);	//关闭8M晶振
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;		//选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//设置引脚模式为推免输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//设置引脚速度为50MHZ
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1);
}

void Low_Power_Config(void)
{
	USART_Cmd(USART1,DISABLE);
	USART_DeInit(USART1);
	
	//LED0_OFF;
	LED1_OFF;
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);	//使能GPIOC的外设时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;			//选择要用的GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//设置引脚模式为上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//设置引脚速度为50MHZ
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0x9FFF;			//PA13、PA14是SWD的引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//进入待机模式
	PWR_EnterSTANDBYMode();
}

int main(void)
{
	LED_GPIO_Config();	//初始化LED
    USART1_Config();	//初始化USART
    RTC_Init();			//初始化RTC
	
    while (1)
    {
		if(standby_counter > 10)
		{
			standby_counter = 0;
			
			Low_Power_Config();
		}
    }
}
