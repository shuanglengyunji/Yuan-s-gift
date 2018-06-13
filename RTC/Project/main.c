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
	LED_GPIO_Config();	//��ʼ��LED
    USART1_Config();	//��ʼ��USART
    RTC_Init();			//��ʼ��RTC
	
    while (1)
    {
		if(standby_counter > 10)
		{
			standby_counter = 0;
			
			LED0_OFF;
			USART_Cmd(USART1, DISABLE);//����USART1
			
			//�������ģʽ
			PWR_EnterSTANDBYMode();
		}
    }
}
