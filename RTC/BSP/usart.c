#include "usart.h"

///初始化USART1
void USART1_Config(void)
{	
    GPIO_InitTypeDef GPIO_InitStructure;	
    USART_InitTypeDef USART_InitStructure;  //定义串口初始化结构体
	
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	/*  USART1_TX -> PA9 */			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//选中串口默认输出管脚         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  	//定义输出最大速率 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//定义管脚9的模式  
	GPIO_Init(GPIOA, &GPIO_InitStructure);           	//调用函数，把结构体参数输入进行初始化		   
	/*  USART1_RX ->	PA10*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200; //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//校验位 无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//使能接收和发送引脚
	USART_Init(USART1, &USART_InitStructure); //将以上赋完值的结构体带入库函数USART_Init进行初始化
	
	USART_Cmd(USART1, ENABLE);//开启USART1
}

///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(USART1, (uint8_t) ch);
	
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		

	return (ch);
}
