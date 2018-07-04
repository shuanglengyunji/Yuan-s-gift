/* Includes ------------------------------------------------------------------*/
#include "RTC_Time.h"

/* Private define ------------------------------------------------------------*/

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
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能RTC和备份区时钟

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);		//允许访问备份区

	/* Reset Backup Domain */
	BKP_DeInit();

	////////////////////////////////////////////////////////////
	
	//将RTC时钟源切换为外部低速时钟源
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
	////////////////////////////////////////////////////////////

	//启动RTC时钟
		
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
		
	////////////////////////////////////////////////////////////

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */	//设置预分频系数

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : RTC_Access_Configuration
* Description    : RTC正常工作时，允许被外部复位的系统访问RTC寄存器.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Access_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能RTC和备份区时钟

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);		//允许访问备份区
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();	//等待RTC时钟同步

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC Initialization
*******************************************************************************/
void RTC_Init(void)
{
	printf("\r\n\r\n\r\n");
	
	// 上电前RTC未工作，需要初始化RTC
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* Backup data register value is not correct or not yet programmed (when
		   the first time the program is executed) */

		printf("上电检测：RTC未初始化\r\n");

		/* RTC Configuration */
		RTC_Configuration();		//配置RTC工作参数（时钟、中断等）

		/* Set RTC Time Value */
		RTC_SetTime(0);
		RTC_WaitForLastTask();
		
		/* Set RTC Alarm Value */
		RTC_SetAlarm(RTC_ALARM_VALUE);
		RTC_WaitForLastTask();

		printf("RTC初始化完成\r\n");

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else	//上电前RTC已经在工作
	{
		printf("上电检测：RTC已经初始化\r\n");
		
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("启动原因：主电源上电r\n");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("启动原因：外部复位\r\n");
		}
		else
		{
			printf("启动原因：RTC闹钟唤醒\r\n");
		}
		
		/* RTC Access Configuration */
		RTC_Access_Configuration();
		
		/* Set RTC Alarm Value */
		RTC_SetAlarm(RTC_ALARM_VALUE);
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//启动RTC秒中断
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
	printf("RTC闹钟触发值为%d，折合%d天\r\n",RTC_ALARM_VALUE,RTC_ALARM_VALUE/3600/24);

	/* NVIC configuration */
	NVIC_Configuration();

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
