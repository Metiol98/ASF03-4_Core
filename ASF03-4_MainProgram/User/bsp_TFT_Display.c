#include "HEAD.h"
/********************************************************************
*屏幕串口：USART1
*
*
*
*
*********************************************************************/
uint8_t TFT_USART_RX_Finish=0;//接收一个完整的数据包。1：接收到一个完整的数据包，0：false
uint8_t TFT_USART_RX_BUF[RX_BUFF_LENGTH]={0};//数据接收缓存器
uint8_t TFT_USART_CodeLength=0;//数据接收缓存器计数

/*****************************************************************
* 名    称： TFT_UART_NVIC_Config()
* 功    能： 屏幕串口优先级分配
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void TFT_UART_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructur;
	//USART1,分组0，抢占0，子优先级14
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);									//分组
	NVIC_InitStructur.NVIC_IRQChannel=TFT_UART_IRQ; 								//通道
	NVIC_InitStructur.NVIC_IRQChannelPreemptionPriority=0;					//抢占优先级，主优先级,查看分组表；
	NVIC_InitStructur.NVIC_IRQChannelSubPriority =1;								//子优先级；
	NVIC_InitStructur.NVIC_IRQChannelCmd =ENABLE;										//开使能；
	NVIC_Init(&NVIC_InitStructur);																	//NVIC初始化；
}

/*****************************************************************
* 名    称： TFT_UART_Config()
* 功    能： 屏幕串口初始化
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void TFT_UART_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructur;					//定义一个GPIO初始化结构体
	USART_InitTypeDef	USART_InitStructur;					//定义一个串口初始化结构体
	
	TFT_UART_GPIO_APBxClkCmd(TFT_UART_GPIO_CLK,ENABLE);			//串口GPIO时钟使能
	TFT_UART_APBxClkCmd(TFT_UART_CLK,ENABLE);								//串口USART1时钟使能
	
	//将USART的TX配置为推挽复用模式
	GPIO_InitStructur.GPIO_Pin=TFT_UART_TX_GPIO_PIN;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructur.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(TFT_UART_TX_GPIO_PORT,&GPIO_InitStructur);
	
	//将USART的RX配置为浮空输入模式
	GPIO_InitStructur.GPIO_Pin=TFT_UART_RX_GPIO_PIN;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(TFT_UART_RX_GPIO_PORT,&GPIO_InitStructur);
	

	//配置串口的工作参数
	USART_InitStructur.USART_BaudRate=TFT_UART_BAUDRATE;											//配置波特率，115200
	USART_InitStructur.USART_WordLength =USART_WordLength_8b;											//配置字长，8位
	USART_InitStructur.USART_StopBits=USART_StopBits_1;														//配置停止位，1位
	USART_InitStructur.USART_Parity=USART_Parity_No;															//配置校验位，无校验
	USART_InitStructur.USART_HardwareFlowControl=USART_HardwareFlowControl_None;	//配置硬件流控制，无控制
	USART_InitStructur.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;										//配置收发模式，收发一起
	
	//完成串口初始化
	USART_Init(TFT_UART,&USART_InitStructur);

	//使能串口接收中断
	USART_ITConfig(TFT_UART,USART_IT_RXNE,ENABLE);
	
	//使能串口
	USART_Cmd(TFT_UART,ENABLE);
}
void TFT_Display_Init(void)
{
	TFT_UART_NVIC_Config();
	TFT_UART_Config();
}





/*****************************************************************
* 名    称： TFT_UART_IRQHandler()
* 功    能： 屏幕串口中断函数
* 入口参数： 
* 出口参数： 
 *****************************************************************/
uint8_t USART_LCD_RX_Counter=0;//数据接收缓存器计数,用于计数接收到多少个数据
uint8_t Code_Finnal_Counter1=0;//帧尾指针，用于接收帧尾计数
void TFT_UART_IRQHandler(void) //串口1中断服务程序
{
	uint8_t Res;
	if(USART_GetITStatus(TFT_UART, USART_IT_RXNE) != RESET)  //接收中断
	{
		USART_ClearITPendingBit(TFT_UART,USART_IT_RXNE);//清中断标志
		Res =USART_ReceiveData(TFT_UART);	//读取接收到的数据
		if((USART_LCD_RX_Counter==0)&&(Res==0xEE))//是帧头，接收帧头，开始接收后面的数据。
		{
			TFT_USART_RX_BUF[0]=Res;
      USART_LCD_RX_Counter=1;
			USART_ClearITPendingBit(TFT_UART,USART_IT_RXNE);//清中断标志
			return;
		}
		if((USART_LCD_RX_Counter==0)&&(Res!=0xEE))//不是帧头，跳过,返回重新计数
		{
			USART_LCD_RX_Counter=0;
			USART_ClearITPendingBit(TFT_UART,USART_IT_RXNE);//清中断标志
			return;
		}
		
		if((TFT_USART_RX_BUF[0]==0xEE)&&(USART_LCD_RX_Counter!=0))	//如果帧头被正确接收，开始接收后面的数据。
			TFT_USART_RX_BUF[USART_LCD_RX_Counter++]=Res;//接收到的数据放到数据缓存区;
			
		//判断帧尾
		if(Res==0xFF)
		{
			switch (Code_Finnal_Counter1)
			{
				case 2:
					Code_Finnal_Counter1 = 3;
					break; //FF FC FF ?? (最后一个字节不对)
				case 3:
					Code_Finnal_Counter1 = 4;
					break; //FF FC FF FF 正确的帧尾
				default:
					Code_Finnal_Counter1 = 1;
					break; //FF ?? ?? ??(最后三个字节不对)
			}
		}
		else if(Res==0xFC)
		{
			switch (Code_Finnal_Counter1)
			{
				case 1:
					Code_Finnal_Counter1 = 2;
					break; //FF FC ?? ??(最后二个字节不对)
				case 3:
					Code_Finnal_Counter1 = 2;
					break; //FF FC FF FC 正确的帧尾
				default:
					Code_Finnal_Counter1 = 0;
					break; //?? ?? ?? ??(全部字节不对)
			}
		}
		else
				Code_Finnal_Counter1 = 0;
		
		if(Code_Finnal_Counter1==4)
		{
			Code_Finnal_Counter1=0;
			TFT_USART_CodeLength=USART_LCD_RX_Counter;//指令的长度
			USART_LCD_RX_Counter=0;//接收指针清零，可以接收新的数据
			TFT_USART_RX_Finish=1;//完整帧就收完成，该指令可以使用
			
			//【停止按钮】
			if((WorkBusy==1)&&(TFT_USART_RX_BUF[4]==0)&&(TFT_USART_RX_BUF[6]==13))//判断指令是否来自于开始按钮
				Emergency_Stop();
			if((WorkBusy==2)&&(TFT_USART_RX_BUF[4]==0)&&(TFT_USART_RX_BUF[6]==2))//判断指令是否来自于工作界面填充
				Emergency_Stop();
			if((WorkBusy==3)&&(TFT_USART_RX_BUF[4]==0)&&(TFT_USART_RX_BUF[6]==3))//判断指令是否来自于工作界面回吸
				Emergency_Stop();
			if((WorkBusy==4)&&(TFT_USART_RX_BUF[4]==1)&&(TFT_USART_RX_BUF[6]==29))//判断指令是否来自于校准界面填充
				Emergency_Stop();
			if((WorkBusy==5)&&(TFT_USART_RX_BUF[4]==1)&&(TFT_USART_RX_BUF[6]==30))//判断指令是否来自于校准界面加液
				Emergency_Stop();
			if((WorkBusy==6)&&(TFT_USART_RX_BUF[4]==1)&&(TFT_USART_RX_BUF[6]==31))//判断指令是否来自于校准界面清洗
				Emergency_Stop();
		}
  }
	USART_ClearITPendingBit(TFT_UART,USART_IT_RXNE);//清中断标志
	return;
}


/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数：ch  发送的字节
* 出口参数： 无
 *****************************************************************/
void SendChar(u8 ch)
{
	USART_SendData(TFT_UART,ch);
	while(USART_GetFlagStatus(TFT_UART,USART_FLAG_TXE)==0);//等待发送成功的标志位。
}
/*****************************************************************
* 名    称： SendStrings()
* 功    能： 发送字符串
* 入口参数： str  发送的字符串
* 出口参数： 无
 *****************************************************************/
void SendStrings(char *str)
{
	while(*str)
	{
		SendChar(*str);
		str++;
	}
}


/*******************************屏幕功能函数*******************************/
/*****************************************************************
* 名    称： SendWrokMode(u8 TFT_Screen_ID, u8 Control_ID, u8 WorkModeNum)
* 功    能： 隐藏显示控件
* 入口参数： TFT_Screen_ID：画面编号，TFT_CTL_ID：控件编号，WorkModeNum:工作模式标志位编号
* 出口参数： 无
 *****************************************************************/
void SendWrokMode(u8 TFT_Screen_ID, u8 Control_ID, u8 WorkModeNum)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x10);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(WorkModeNum);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
}

/*****************************************************************
* 名    称： DisplayControl(u8 TFT_Screen_ID, u8 Control_ID,u8 hide)
* 功    能： 隐藏显示控件
* 入口参数： TFT_Screen_ID：画面编号，TFT_CTL_ID：控件编号，hide:1显示，0隐藏
* 出口参数： 无
 *****************************************************************/
void DisplayControl(u8 TFT_Screen_ID, u8 Control_ID,u8 hide)
{

	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x03);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	if(hide==0)
		SendChar(0x00);//隐藏
	else
		SendChar(0x01);//显示
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);	
	//Systick_Delay_ms(2);
}

/*****************************************************************
* 名    称： SetTextValue()
* 功    能： 更新触摸屏文本控件数值
* 入口参数： TFT_Screen_ID：画面编号，TFT_CTL_ID：控件编号，str：发送的字符串
* 出口参数： 无
 *****************************************************************/
void SetTextValue(u8 TFT_Screen_ID,u16 TFT_CTL_ID,char *str)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x10);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(TFT_CTL_ID);
	SendStrings(str);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	//Systick_Delay_ms(2);
}

/*****************************************************************
* 名    称： Set_Set_ProgressValue(u8 TFT_Screen_ID,u8 Progres)
* 功    能： 更新进度条数值
* 入口参数： TFT_Screen_ID：画面编号，TFT_CTL_ID：控件编号，Progres:进度条数值
* 出口参数： 无
*****************************************************************/
//void Set_ProgressValue(u8 TFT_Screen_ID, u8 Control_ID,u8 Progres)
//{
//	SendChar(0xEE);
//	SendChar(0xB1);
//	SendChar(0x00);
//	SendChar(TFT_Screen_ID);
//	SendChar(0x00);
//	SendChar(Control_ID);
//	SendChar(0x00);
//	SendChar(0x00);
//	SendChar(0x00);
//	SendChar(Progres);
//	SendChar(0xFF);
//	SendChar(0xFC);
//	SendChar(0xFF);
//	SendChar(0xFF);		
//}

/*****************************************************************
* 名    称： SwitchReSet(u8 TFT_Screen_ID,u8 Progres)
* 功    能： 弹起按钮
* 入口参数： TFT_Screen_ID：画面编号，TFT_CTL_ID：控件编号，Progres:进度条数值
* 出口参数： 无
*****************************************************************/
void SwitchReSet(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x10);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0x00);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	//Systick_Delay_ms(2);
}

 /*****************************************************************
* 名    称： SwitchSet(u8 TFT_Screen_ID,u8 Progres)
* 功    能： 按下按钮
* 入口参数： TFT_Screen_ID：画面编号，TFT_CTL_ID：控件编号，Progres:进度条数值
* 出口参数： 无
*****************************************************************/
void SwitchSet(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x10);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0x01);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	//Systick_Delay_ms(2);
}

 /*****************************************************************
* 名    称： SwitchScreen(u8 TFT_Screen_ID)
* 功    能： 按下按钮
* 入口参数： 切换画面
* 出口参数： 无
*****************************************************************/
void SwitchScreen(u8 TFT_Screen_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x00);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	//Systick_Delay_ms(2);
}
 /*****************************************************************
* 名    称： ReadTextValue(u8 TFT_Screen_ID)
* 功    能： 按下按钮
* 入口参数： 切换画面
* 出口参数： 无
*****************************************************************/
void ReadTextValue(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x11);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	//Systick_Delay_ms(2);
}
 /*****************************************************************
* 名    称： StarTimer(u8 TFT_Screen_ID)
* 功    能： 开始计时
* 入口参数： 
* 出口参数： 无
*****************************************************************/
void StarTimer(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x41);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	Systick_Delay_ms(2);
}
 /*****************************************************************
* 名    称： StopTimer(u8 TFT_Screen_ID)
* 功    能： 暂停计时
* 入口参数： 
* 出口参数： 无
*****************************************************************/
void StopTimer(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x44);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	Systick_Delay_ms(2);
}
 /*****************************************************************
* 名    称： ClearTimer(u8 TFT_Screen_ID)
* 功    能： 清零计时
* 入口参数： 
* 出口参数： 无
*****************************************************************/
void ClearTimer(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x40);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0x00);
	SendChar(0x00);
	SendChar(0x0E);
	SendChar(0x10);	
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	Systick_Delay_ms(2);
}
 /*****************************************************************
* 名    称： ReadTimer(u8 TFT_Screen_ID)
* 功    能： 读取定时器时间
* 入口参数： 
* 出口参数： 无
*****************************************************************/
void ReadTimer(u8 TFT_Screen_ID, u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x45);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);	
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	Systick_Delay_ms(2);
}

 /*****************************************************************
* 名    称： ScreenCalibration(void)
* 功    能： 读取定时器时间
* 入口参数： 
* 出口参数： 无
*****************************************************************/
void ScreenCalibration(void)
{
	SendChar(0xEE);
	SendChar(0x72);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
	Systick_Delay_ms(2);
}

/*****************************************************************
* 名    称： SwitchReadTextValue()
* 功    能： 初始化时向title里发送文字类容
* 入口参数：
* 出口参数： 
 *****************************************************************/
void SendTitleTextValue(uint32_t TitleFlag, u8 TFT_Screen_ID, u8 Control_ID)
{
	switch(TitleFlag)
	{
		case 0://盐酸
			SetTextValue(TFT_Screen_ID,Control_ID,"盐酸");
			break;
		case 1://硝酸
			SetTextValue(TFT_Screen_ID,Control_ID,"硝酸");
			break;
		case 2://氢氟酸
			SetTextValue(TFT_Screen_ID,Control_ID,"氢氟酸");
			break;
		case 3://高氯酸
			SetTextValue(TFT_Screen_ID,Control_ID,"高氯酸");
			break;
		case 4://硫酸
			SetTextValue(TFT_Screen_ID,Control_ID,"硫酸");
			break;
		case 5://其他
			SetTextValue(TFT_Screen_ID,Control_ID,"其他");
			break;
		default:
			SetTextValue(TFT_Screen_ID,Control_ID,"   ");

			break;
	}
	return;
}

/*****************************************************************
* 名    称： DataRecode_ClearRecode(u8 TFT_Screen_ID,u8 Control_ID)             
* 功    能： 数据记录控件-清除数据记录
* 入口参数：
*           TFT_Screen_ID：画面编号
*           TFT_CTL_ID：控件编号
* 出口参数： 无
 *****************************************************************/		
void DataRecode_ClearRecode(u8 TFT_Screen_ID,u8 Control_ID)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x53);
	SendChar(0x00);	
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
}

/*****************************************************************
* 名    称： DataRecode_Add1Line(u8 TFT_Screen_ID,u8 TFT_CTL_ID,char *str);                                    
* 功    能： 数据记录控件-添加1行记录，最大32个字符/16个汉字
* 入口参数：
*           TFT_Screen_ID：画面编号
*           TFT_CTL_ID：控件编号
*           str:       待发送的字符串
* 出口参数： 无
 *****************************************************************/		
void DataRecode_Add1Line(u8 TFT_Screen_ID,u8 Control_ID, char *str)
{
	SendChar(0xEE);
	SendChar(0xB1);
	SendChar(0x52);
	SendChar(0x00);
	SendChar(TFT_Screen_ID);
	SendChar(0x00);
	SendChar(Control_ID);
	SendStrings(str);
	SendChar(0xFF);
	SendChar(0xFC);
	SendChar(0xFF);
	SendChar(0xFF);
}

