/* Includes ------------------------------------------------------------------*/
#include "RTC_Time.h" 

/* Private define ------------------------------------------------------------*/

//#define RTCClockOutput_Enable		/* RTC Clock/64 is output on tamper pin(PC.13) */

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : 获取RTC计数器中的值
* Return         : uint32_t
*******************************************************************************/
uint32_t RTC_GetTime(void)
{
	return RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : RTC_SetTime
* Description    : 设置RTC计数器里的值
* Input          : uint32_t
*******************************************************************************/
void RTC_SetTime(uint32_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter(t);
	RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC Initialization
*******************************************************************************/
void RTC_Init(void)
{

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* Backup data register value is not correct or not yet programmed (when
		   the first time the program is executed) */

		printf("\r\n\r\nRTC未初始化....\r\n\r\n");

		/* RTC Configuration */
		RTC_Configuration();		//配置RTC工作参数（时钟、中断等）

		RTC_SetTime(0);

		/* Adjust time by values entred by the user on the hyperterminal */

		printf("RTC初始化完成....\r\n\r\n");

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n\r\n发生一次电源上电重启....\r\n\r\n");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n\r\n发生一次外部复位....\r\n\r\n");
		}

		printf("\r\n\r\n不需要设置RCT....\r\n\r\n");
		
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}

	/* NVIC configuration */
	NVIC_Configuration();

	#ifdef RTCClockOutput_Enable
		/* Enable PWR and BKP clocks */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

		/* Allow access to BKP Domain */
		PWR_BackupAccessCmd(ENABLE);

		/* Disable the Tamper Pin */
		BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
								 functionality must be disabled */

		/* Enable RTC Clock Output on Tamper Pin */
		BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif

	/* Clear reset flags */
	RCC_ClearFlag();

}

/*******************************************************************************
* Function Name  : Time_Display
* Description    : Printf Time
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_Display(void)
{
	uint32_t time, day, hour, minute, second;
	
	time = RTC_GetTime();
	
	printf("运行总秒数：%d，",time);
	
	day = time / 86400;		//天
	time = time % 86400;	//不足1天的剩余秒数
	hour = time / 3600;		//小时
	time = time % 3600;		//不足1小时的剩余秒数
	minute = time / 60;		//分钟
	second = time % 60;		//秒
	
	printf("折合%d天，%d小时，%d分钟，%d秒\r\n", day, hour, minute, second);
}
