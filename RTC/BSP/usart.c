#include "usart.h"

///��ʼ��USART1
void USART1_Config(void)
{	
    GPIO_InitTypeDef GPIO_InitStructure;	
    USART_InitTypeDef USART_InitStructure;  //���崮�ڳ�ʼ���ṹ��
	
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	/*  USART1_TX -> PA9 */			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//ѡ�д���Ĭ������ܽ�         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  	//�������������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//����ܽ�9��ģʽ  
	GPIO_Init(GPIOA, &GPIO_InitStructure);           	//���ú������ѽṹ�����������г�ʼ��		   
	/*  USART1_RX ->	PA10*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200; //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//ֹͣλ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;		//У��λ ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//ʹ�ܽ��պͷ�������
	USART_Init(USART1, &USART_InitStructure); //�����ϸ���ֵ�Ľṹ�����⺯��USART_Init���г�ʼ��
	
	USART_Cmd(USART1, ENABLE);//����USART1
}

///�ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(USART1, (uint8_t) ch);
	
	/* �ȴ�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		

	return (ch);
}
