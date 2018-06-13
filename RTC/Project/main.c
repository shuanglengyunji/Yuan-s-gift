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
			
			LED0_OFF;
			USART_Cmd(USART1, DISABLE);//开启USART1
			
			//进入待机模式
			PWR_EnterSTANDBYMode();
		}
    }
}
