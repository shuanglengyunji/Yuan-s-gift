#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
void RTC_Init(void);
void Time_Display(void);
void Time_Regulate(void);

/* Private variables ---------------------------------------------------------*/
extern FunctionalState TimeDisplay;

#endif
