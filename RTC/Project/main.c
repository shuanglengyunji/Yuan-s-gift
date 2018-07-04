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
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//定时结束处理
	u32 time = RTC_GetCounter();
	if(time >= RTC_ALARM_VALUE)		//定时时间已到
	{
		printf("\r\n定时时间已到\r\n");
		
		//所有端口改为模拟输入
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);	//使能GPIOC的外设时钟
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//设置引脚模式为上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//设置引脚速度为50MHZ
		GPIO_InitStructure.GPIO_Pin = 0xFFFF;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0xFFFF;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x9FFF;			//PA13、PA14是SWD的引脚
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, DISABLE);
		
		//LED灯对应引脚使能
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	//使能GPIOC的外设时钟
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//设置引脚模式为上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//设置引脚速度为50MHZ
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		// 开LED
		LED0_ON;
		LED1_ON;
		
		USART_Cmd(USART1,DISABLE);
		USART_DeInit(USART1);
		
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);	//用这个stop模式吧，这个模式下led能亮
	}
	else
	{
		// 关LED
		LED0_OFF;
		LED1_OFF;
		
		//所有端口改为模拟输入
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);	//使能GPIOC的外设时钟
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//设置引脚模式为上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//设置引脚速度为50MHZ
		GPIO_InitStructure.GPIO_Pin = 0xFFFF;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0xFFFF;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x9FFF;			//PA13、PA14是SWD的引脚
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, DISABLE);
		
		USART_Cmd(USART1,DISABLE);
		USART_DeInit(USART1);
	
		//进入待机模式
		PWR_EnterSTANDBYMode();
	}
}

int main(void)
{
	LED_GPIO_Config();	//初始化LED
    USART1_Config();	//初始化USART
    RTC_Init();			//初始化RTC
	
    while (1)
    {
		if(standby_counter >= 1)
		{
			standby_counter = 0;
			
			Low_Power_Config();
		}
    }
}
