#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

// RTC���ӻ���ֵ
#define RTC_ALARM_VALUE	42949673	// 2^32 / 100 = 42949672.96

/* Private function prototypes -----------------------------------------------*/
void RTC_Init(void);
void Time_Display(void);
void Time_Regulate(void);

/* Private variables ---------------------------------------------------------*/
extern FunctionalState TimeDisplay;

#endif
