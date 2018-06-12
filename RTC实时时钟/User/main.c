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

/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint32_t TimeDisplay = 0;

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

	/* 进入工作循环 */
	Time_Show(&systmtime);
}

/*
 * 函数名：Time_Show
 * 描述  ：在超级终端中显示当前时间值
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */ 
void Time_Show(struct rtc_time *tm)
{
	uint32_t rtc_time;
	while (1)
	{
		/* 每过1s */
		if (TimeDisplay == 1)
		{
			RTC_GetCounter();
			printf("RTC已经运行%d秒",rtc_time);
			
			TimeDisplay = 0;	//清标志位
		}
	}
}

/***********************************END OF FILE*********************************/

