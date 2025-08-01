#include "HEAD.h"
/*******接收码******/
u8 SRX885_UART_RX_BUF[20];

//ASCII: K0: LC:8510A83D    有效数据位 85 10 A8 3D为验证码 16进制码：0x4C 0x43 0x3A 0x85 0x10 0xA8 0X3D  
//       K1:

/*****************************************************************
* 名    称： TFT_UART_NVIC_Config()
* 功    能： 屏幕串口优先级分配
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void SRX885_UART_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructur;
	//USART1,分组0，抢占0，子优先级14
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);									//分组
	NVIC_InitStructur.NVIC_IRQChannel=SRX885_UART_IRQ; 								//通道
	NVIC_InitStructur.NVIC_IRQChannelPreemptionPriority=1;					//抢占优先级，主优先级,查看分组表；
	NVIC_InitStructur.NVIC_IRQChannelSubPriority =3;								//子优先级；
	NVIC_InitStructur.NVIC_IRQChannelCmd =ENABLE;										//开使能；
	NVIC_Init(&NVIC_InitStructur);																	//NVIC初始化；
}


/*****************************************************************
* 名    称： TFT_UART_Config()
* 功    能： 屏幕串口初始化
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void SRX885_UART_Config(void)
{
	GPIO_InitTypeDef		GPIO_InitStructur;					//定义一个GPIO初始化结构体
	USART_InitTypeDef		USART_InitStructur;					//定义一个串口初始化结构体
	
	SRX885_UART_GPIO_APBxClkCmd(SRX885_UART_GPIO_CLK,ENABLE);			//串口GPIO时钟使能
	SRX885_UART_APBxClkCmd(SRX885_UART_CLK,ENABLE);								//串口USART1时钟使能
	
	//将USART的TX配置为推挽复用模式
//	GPIO_InitStructur.GPIO_Pin=TFT_UART_TX_GPIO_PIN;
//	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_AF_PP;
//	GPIO_InitStructur.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_Init(TFT_UART_TX_GPIO_PORT,&GPIO_InitStructur);
	
	//将USART的RX配置为浮空输入模式
	GPIO_InitStructur.GPIO_Pin=SRX885_UART_RX_GPIO_PIN;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(SRX885_UART_RX_GPIO_PORT,&GPIO_InitStructur);
	

	//配置串口的工作参数
	USART_InitStructur.USART_BaudRate=SRX885_UART_BAUDRATE;											//配置波特率，115200
	USART_InitStructur.USART_WordLength =USART_WordLength_8b;											//配置字长，8位
	USART_InitStructur.USART_StopBits=USART_StopBits_1;														//配置停止位，1位
	USART_InitStructur.USART_Parity=USART_Parity_No;															//配置校验位，无校验
	USART_InitStructur.USART_HardwareFlowControl=USART_HardwareFlowControl_None;	//配置硬件流控制，无控制
	USART_InitStructur.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;										//配置收发模式，收发一起
	
	//完成串口初始化
	USART_Init(SRX885_UART,&USART_InitStructur);

	//使能串口接收中断
	USART_ITConfig(SRX885_UART,USART_IT_RXNE,ENABLE);
	
	//使能串口
	USART_Cmd(SRX885_UART,ENABLE);
}	

void SRX885_UART_Init(void)
{
	SRX885_UART_NVIC_Config();
	SRX885_UART_Config();
}



/*****************************************************************
* 名    称： SRX885_UART_IRQHandler()
* 功    能： 中断接收函数
* 入口参数： 
* 出口参数： 
 *****************************************************************/
uint32_t SRX885_Cmd=0;//接收计数
uint32_t SRX885Counter=0;//接收计数,防止按键粘连
uint8_t SRX885_USART_RX_Finish=0;//接收完成标志位
uint8_t WorkBusy=0;//0表示空闲，1表示忙碌
uint8_t SRX885_Button_1=0;
uint8_t SRX885_Button_2=0;
uint8_t SRX885_Finish=0;//
void SRX885_UART_IRQHandler(void)
{  
	uint8_t res;
	if (USART_GetFlagStatus(SRX885_UART, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(SRX885_UART);//清中断标志
		USART_ClearFlag(SRX885_UART, USART_FLAG_ORE);
	}
	if(USART_GetITStatus(SRX885_UART, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(SRX885_UART,USART_IT_RXNE);//清中断标志
		USART_ClearFlag(SRX885_UART, USART_IT_RXNE);
		res=USART_ReceiveData(SRX885_UART);
		SRX885_UART_RX_BUF[SRX885_Cmd++]=res;

		if(SRX885_Cmd>=20)
		{
			SRX885_Cmd=0;
		}

//		//if(SRX885_UART_RX_BUF[12]==0x0A)//按键有效
		if(SRX885_Cmd>=8)//按键有效
		{
			SRX885_Cmd=0;
			SRX885_UART_RX_BUF[12]=0;
			
			SRX885Counter=0;//计数归零，防止按键粘连重复进入，
			
			if(SRX885_Finish==1)//可以重新进入按键程序
			{
				SRX885_Finish=0;//可以重新进入标志位归零
				if(WorkBusy==0)//空闲时按下
				{
					if(ScreenState==0)//0在工作界面，1在校准界面。
					{
						TFT_USART_RX_BUF[4]=0;
						TFT_USART_RX_BUF[6]=13;
						TFT_USART_RX_Finish=1;
					}
					if(ScreenState==1)//0在工作界面，1在校准界面。
					{
						TFT_USART_RX_BUF[4]=1;
						TFT_USART_RX_BUF[6]=30;
						TFT_USART_RX_Finish=1;
					}
				}
				if(WorkBusy!=0)//忙碌时按下
				{
					Emergency_Stop_Flag=1;//急停打开
					Emergency_Stop();
					Add_IntervalTimeCounter=0;//间隔时间相关计数标志位清零
					Add_IntervalTimeFlag=1;//循环模式在急停下，标志位置1，跳出等待循环
				}
			}
		}
	}
	USART_ClearITPendingBit(SRX885_UART,USART_IT_RXNE);//清中断标志
	USART_ClearFlag(SRX885_UART, USART_IT_RXNE);
	return;
}	



