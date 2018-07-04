/* Includes ------------------------------------------------------------------*/
#include "RTC_Time.h"

/* Private define ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : ��ȡRTC�������е�ֵ
* Return         : uint32_t
*******************************************************************************/
uint32_t RTC_GetTime(void)
{
	return RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : RTC_SetTime
* Description    : ����RTC���������ֵ
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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��RTC�ͱ�����ʱ��

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);		//������ʱ�����

	/* Reset Backup Domain */
	BKP_DeInit();

	////////////////////////////////////////////////////////////
	
	//��RTCʱ��Դ�л�Ϊ�ⲿ����ʱ��Դ
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
	////////////////////////////////////////////////////////////

	//����RTCʱ��
		
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
		
	////////////////////////////////////////////////////////////

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */	//����Ԥ��Ƶϵ��

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : RTC_Access_Configuration
* Description    : RTC��������ʱ�������ⲿ��λ��ϵͳ����RTC�Ĵ���.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Access_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��RTC�ͱ�����ʱ��

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);		//������ʱ�����
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();	//�ȴ�RTCʱ��ͬ��

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
	
	// �ϵ�ǰRTCδ��������Ҫ��ʼ��RTC
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* Backup data register value is not correct or not yet programmed (when
		   the first time the program is executed) */

		printf("�ϵ��⣺RTCδ��ʼ��\r\n");

		/* RTC Configuration */
		RTC_Configuration();		//����RTC����������ʱ�ӡ��жϵȣ�

		/* Set RTC Time Value */
		RTC_SetTime(0);
		RTC_WaitForLastTask();
		
		/* Set RTC Alarm Value */
		RTC_SetAlarm(RTC_ALARM_VALUE);
		RTC_WaitForLastTask();

		printf("RTC��ʼ�����\r\n");

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else	//�ϵ�ǰRTC�Ѿ��ڹ���
	{
		printf("�ϵ��⣺RTC�Ѿ���ʼ��\r\n");
		
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("����ԭ������Դ�ϵ�r\n");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("����ԭ���ⲿ��λ\r\n");
		}
		else
		{
			printf("����ԭ��RTC���ӻ���\r\n");
		}
		
		/* RTC Access Configuration */
		RTC_Access_Configuration();
		
		/* Set RTC Alarm Value */
		RTC_SetAlarm(RTC_ALARM_VALUE);
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//����RTC���ж�
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
	printf("RTC���Ӵ���ֵΪ%d���ۺ�%d��\r\n",RTC_ALARM_VALUE,RTC_ALARM_VALUE/3600/24);

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
	
	printf("������������%d��",time);
	
	day = time / 86400;		//��
	time = time % 86400;	//����1���ʣ������
	hour = time / 3600;		//Сʱ
	time = time % 3600;		//����1Сʱ��ʣ������
	minute = time / 60;		//����
	second = time % 60;		//��
	
	printf("�ۺ�%d�죬%dСʱ��%d���ӣ�%d��\r\n", day, hour, minute, second);
}
