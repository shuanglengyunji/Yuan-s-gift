/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc 测试，显示时间格式为: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_rtc.h"

// N = 2^32/365/24/60/60 = 136 年

/*时间结构体*/
struct rtc_time systmtime;

/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int main()
{
	RTC_NVIC_Config();		//初始化中断向量控制器
	USART1_Config();		//初始化USART1，不开启中断
	RTC_CheckAndConfig(&systmtime);		//配置RTC，产生秒中断

	/* Display time in infinite loop */
	Time_Show(&systmtime);
}

/***********************************END OF FILE*********************************/

