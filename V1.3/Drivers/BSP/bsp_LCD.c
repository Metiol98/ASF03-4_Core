#include "bsp_LCD.h"

RingBufferDef RingBuffer;
TFTDisplaySTR  g_TFTDisplay;

static uint8_t LCD_FH[] = {0xEE};		//LCD发送帧头
static uint8_t LCD_FT[] = {0xFF,0xFC,0xFF,0xFF};		//LCD发送帧尾

void LCD_init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,g_TFTDisplay.RxBuffer,bsp_TFT_USART_RX_BUFFER_SIZE);
}

uint8_t LCD_CheckData(uint8_t *data,uint8_t len)
{
	uint8_t right_flag = 1;
	
	if( (len>5) && (data[0]==LCD_FH[0])
			&&(data[len-4]==LCD_FT[0])
			&&(data[len-3]==LCD_FT[1])
			&&(data[len-2]==LCD_FT[2])
			&&(data[len-1]==LCD_FT[3])
			&&(len>5)	)
		return right_flag;	//数据正确
	else
		return !right_flag;	//数据错误
}

/***************** 屏幕功能函数 --START *************/
/**
	* @brief  ASCII码转一位十进制
	* @param  *array：需要转换的值
	* @retval 返回转换的int类型数据.
	*/
int TFTDisplay_ASCIItoDEC(uint8_t *arry)
{
	if( (*arry>=0x30) && (*arry<=0x39) )		
		return (*arry - 0x30);
	else
		return 0;
}

/**
	* @brief  ASCII码转一位十进制，可以识别负数
	* @param  *array：需要转换的字符串
	* @retval 返回转换的float类型数据.
	*/
float TFTDisplay_ASCIItoFloat(uint8_t *Arry)
{
	float data = 0;					 /* 整数暂存 */
	float decimal_1;				 /* 小数的整数部分暂存 */
	float decimal_0;				 /* 小数的小数部分暂存 */
	int dataLength_bump = 8; /* 数据位数基数标志位 */
	int dataLength;					 /* 数据的总位数，包括小数点 */
	int point = 0;					 /* 小数的整数位数 */

	while (Arry[dataLength_bump] != 0x00)
	{
		if (Arry[dataLength_bump] == 0x2E)
			point = dataLength_bump - 8; /* 表示有小数，且poin为整数部分的位数 */
		dataLength_bump++;						 /* 判断数据位数，包含小数点 */
	}
	dataLength = dataLength_bump - 8; /* 总长度 */

	if (point != 0)
	{								 /* 是小数 */
		decimal_1 = 0; /* 整数部分 */
		for (int i = 0; i < point; i++)
			decimal_1 = decimal_1 * 10 + TFTDisplay_ASCIItoDEC(&Arry[8 + i]);

		decimal_0 = 0; /* 小数部分 */
		for (int i = (dataLength - point - 1); i > 0; i--)
			decimal_0 = decimal_0 * 0.1 + TFTDisplay_ASCIItoDEC(&Arry[8 + point + i]);
		decimal_0 = decimal_0 * 0.1;

		if (Arry[8] != 0x2D)
			return decimal_1 + decimal_0;
		else
			return -(decimal_1 + decimal_0);
	}

	else
	{ /* 是整数 */
		data = 0;
		for (int i = 0; i < dataLength; i++)
			data = data * 10 + TFTDisplay_ASCIItoDEC(&Arry[8 + i]);

		if (Arry[8] != 0x2D)
			return data;
		else
			return -data;
	}
}

/**
	* @brief  TFT_USART发送字节
	* @param  byte	要发送字节
	* @retval None.
	*/
void bsp_TFT_USART_SendByte(uint8_t byte)
{
	HAL_UART_Transmit(&LCD_UART,&byte,1,50);
}

/**
	* @brief  TFT_USART发送字符串
	* @param  byte	要发送字符串
	* @retval None.
	*/
void bsp_TFT_USART_SendStrings(char *str)
{
	while(*str)
	{
		bsp_TFT_USART_SendByte(*str);
		str++;
	}
}

/**
	* @brief  TFT_USART发送数组
	* @param  *array ：要发送数组
	* @param  num 	 ：数组大小
	* @retval None.
	*/
void bsp_TFT_USART_SendArray(uint8_t *array, uint8_t num)
{
	for(int i=0;i<num;i++)
	{
		bsp_TFT_USART_SendByte(array[i]);
	}
}

/**
	* @brief  隐藏显示控件
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param 	hide						：标志位
			@arg	1	：显示
			@arg	0	：隐藏
	* @retval None.
	*/
void TFTDisplay_DisplayControl(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID,uint8_t hide)
{
	uint8_t cmd[] = {0xEE,0xB1,0x03,0x00,0x00,0x00,0x00,0x00,0xFF,0xFC,0xFF,0xFF};
	
	cmd[3] = TFT_Screen_ID>>8;
	cmd[4] = TFT_Screen_ID;
	cmd[5] = TFT_Control_ID>>8;
	cmd[6] = TFT_Control_ID;
	cmd[7] = hide ? 1 : 0;
	bsp_TFT_USART_SendArray(cmd, sizeof(cmd));
}

/**
	* @brief  文字控件显示文字信息
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param 	*str						：显示的文字
	* @retval None.
	*/
void TFTDisplay_SetTextValue(uint16_t TFT_Screen_ID,uint16_t TFT_Control_ID,char *str)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x10);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendStrings(str);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  设置按钮控件弹起和按下状态
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param 	sta							：控件状态
			@arg	1	：按下
			@arg	0	：弹起
	* @retval None.
	*/
void TFTDisplay_SwitchControl(uint16_t TFT_Screen_ID,uint16_t TFT_Control_ID,uint8_t sta)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x10);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	if(sta)
		bsp_TFT_USART_SendByte(0x01);		//按下
	else
		bsp_TFT_USART_SendByte(0x00);		//弹起
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  TFT切换画面
	* @param  TFT_Screen_ID		：画面编号（两位 例: 00 01）
	* @retval None.
	*/
void TFTDisplay_SwitchScreen(uint8_t TFT_Screen_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  读取文本
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @retval None.
	*/
void TFTDisplay_ReadTextValue(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x11);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  清除文本
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @retval None.
	*/
void TFTDisplay_ClearTextValue(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x10);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  定时器开始计时
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @retval None.
	*/
void TFTDisplay_StarTimer(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x41);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  定时器暂停计时
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @retval None.
	*/
void TFTDisplay_StopTimer(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x44);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  读取定时器时间
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @retval None.
	*/
void TFTDisplay_ReadTimer(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x45);
	bsp_TFT_USART_SendByte(TFT_Screen_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(TFT_Control_ID>>8);
	bsp_TFT_USART_SendByte(TFT_Control_ID);	
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
	SysTickDelay_ms(2);
}

/**
	* @brief  获取屏幕的RTC时间
	* @param  *data		：返回数据保存地址的指针
	* @retval None.
	*/
static volatile TFTDateTime s_rtc_date;
static volatile uint32_t s_rtc_received_at;
static volatile uint8_t s_rtc_received;

void TFTDisplay_RequestRTC(void)
{
	static const uint8_t command[] = {0xEE, 0x82, 0xFF, 0xFC, 0xFF, 0xFF};
	for (unsigned i = 0; i < sizeof(command); i++)
		bsp_TFT_USART_SendByte(command[i]);
}

uint8_t TFTDisplay_ReadRTC(TFTDateTime *date, uint32_t *received_at)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	uint8_t received = s_rtc_received;
	if (received != 0U)
	{
		*date = s_rtc_date;
		*received_at = s_rtc_received_at;
	}
	if (primask == 0U)
		__enable_irq();
	return received;
}

static void LCD_ReceiveRTC(const uint8_t *frame)
{
	TFTDateTime date;
	uint8_t values[7];
	for (unsigned i = 0; i < 7U; i++)
	{
		uint8_t bcd = frame[2U + i];
		if (((bcd >> 4) > 9U) || ((bcd & 15U) > 9U))
			return;
		values[i] = (bcd >> 4) * 10U + (bcd & 15U);
	}
	date.year = values[0];
	date.month = values[1];
	date.day = values[3];
	date.hour = values[4];
	date.minute = values[5];
	date.second = values[6];
	s_rtc_date = date;
	s_rtc_received_at = HAL_GetTick();
	s_rtc_received = 1U;
}

/**
	* @brief  数据记录控件 -- 清除数据记录
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @retval None.
	*/
void TFTDisplay_DataRecodeClearRecode(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x53);
	bsp_TFT_USART_SendByte(0x00);	
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  数据记录控件 -- 添加一行记录，最大32个字符/16个汉字
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param  *str						：待发送的字符串
	* @retval None.
	*/
void TFTDisplay_DataRecodeAddLine(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID, char *str)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x52);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendStrings(str);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  数据记录控件 -- 隐藏/显示控件
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param	hide
			@arg	1	：显示
			@arg	0	：隐藏
	* @retval None.
	*/
void TFTDisplay_DataRecodeDisplayControl(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID,uint8_t hide)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x03);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	if(hide)
		bsp_TFT_USART_SendByte(0x01);
	else
		bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  滑块组件，发送数据
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param	value						：值
	* @retval None.
	*/
void TFTDisplay_Slider(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID, uint8_t value)
{
	bsp_TFT_USART_SendByte(0xEE);
	bsp_TFT_USART_SendByte(0xB1);
	bsp_TFT_USART_SendByte(0x10);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Screen_ID);
	bsp_TFT_USART_SendByte(0x00);
	bsp_TFT_USART_SendByte(TFT_Control_ID);
	bsp_TFT_USART_SendByte(value);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFC);
	bsp_TFT_USART_SendByte(0xFF);
	bsp_TFT_USART_SendByte(0xFF);
}

/**
	* @brief  曲线控件，缩放0-1000
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param	value						：值
	* @param	scale						：值
	* @retval None.
	*/
void TFTDisplay_CurveControl(uint8_t TFT_Screen_ID,uint8_t Control_ID, float value, float Scale)
{
	/* EE B1 32 00 01 00 01 00 00 02 03 E8 FF FC FF FF  */
	uint16_t valuetemp = 0;
	uint8_t cmd[]={	0xEE,0xB1,0x32,
									0x00,0x01,	/* 屏幕ID */
									0x00,0x01,	/* 控件ID */
									0x00,				/* 曲线通道 */
									0x00,0x02,
									0x00,0x00,	/* 数据 */
									0xFF,0xFC,0xFF,0xFF};
	
	/* 数据缩放 */
	cmd[4] = TFT_Screen_ID;		
	cmd[6] = Control_ID;									
									
	valuetemp = (value*427)/Scale;
	cmd[10] = valuetemp >> 8;
	cmd[11] = valuetemp & 0x00FF;

	for(int i=0;i<sizeof(cmd);i++)
		bsp_TFT_USART_SendByte(cmd[i]);
}

/**
	* @brief  曲线控件的缩放平移
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param	HorOffset				：水平偏移
	* @param	HorScal					：水平缩放
	* @param	VerOffset				：垂直偏移
	* @param	VerScal					：垂直缩放
	* @retval None.
	*/
void TFTDisplay_CurveSet(uint8_t TFT_Screen_ID,uint8_t Control_ID, int8_t HorOffset, uint32_t HorScal, int8_t VerOffset, uint32_t VerScal)
{
	/* EE B1 34 00 01 00 00 FF FB 00 5E 00 0A 00 64 FF FC FF FF  */
	uint16_t temp = 0;
	uint8_t cmd[]={	0xEE,0xB1,0x34,
									0x00,0x01,/* 屏幕ID */
									0x00,0x01,/* 控件ID */
									0x00,0x00,/* 水平偏移 */
									0x00,0x64,/* 水平缩放 */
									0x00,0x00,/* 垂直偏移 */
									0x00,0x64,/* 垂直缩放 */
									0xFF,0xFC,0xFF,0xFF};

	/* 数据缩放 */
	cmd[4] = TFT_Screen_ID;
	cmd[6] = Control_ID;

	temp = HorOffset;
	cmd[7] = temp >> 8;
	cmd[8] = temp & 0x00FF;

	temp = HorScal;
	cmd[9] = temp >> 8;
	cmd[10] = temp & 0x00FF;

	temp = VerOffset;
	cmd[11] = temp >> 8;
	cmd[12] = temp & 0x00FF;

	temp = VerScal;
	cmd[13] = temp >> 8;
	cmd[14] = temp & 0x00FF;

	for (int i = 0; i < sizeof(cmd); i++)
		bsp_TFT_USART_SendByte(cmd[i]);
}

/**
	* @brief  初始化时向title里发送文字类容
	* @param  TFT_Screen_ID		：画面编号
	* @param  TFT_Control_ID	：控件编号
	* @param	TitleFlag				：对应的文字
	* @retval None.
	*/
void TFTDisplay_SendTitleTextValue(uint8_t TFT_Screen_ID, uint8_t Control_ID, uint32_t TitleFlag)
{
	switch(TitleFlag)
	{
		case 0://盐酸
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"盐酸");
			break;
		case 1://硝酸
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"硝酸");
			break;
		case 2://氢氟酸
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"氢氟酸");
			break;
		case 3://高氯酸
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"高氯酸");
			break;
		case 4://硫酸
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"硫酸");
			break;
		case 5://其他
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"其他");
			break;
		default:
			TFTDisplay_SetTextValue(TFT_Screen_ID,Control_ID,"   ");
			break;
	}
	return;
}

/***************** 屏幕功能函数 --END *************/

/**
  * @brief  屏幕串口中断函数，串口空闲中断进入此函数，做数据判断处理；
  * @param  None.
  * @retval None.
  */
void LCD_IRQ_Process(void)
{
	uint8_t rx_len=0;	//LCD接受数据长度
	
	rx_len = bsp_TFT_USART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&LCD_DMA_UART_RX);
	uint8_t *frame = g_TFTDisplay.RxBuffer;
	if (LCD_CheckData(frame, rx_len))
	{
		if (frame[1] == 0xF7U)
		{
			if (rx_len == 13U)
				LCD_ReceiveRTC(frame);
		}
		else
		{
			EmergencyStopJudge(frame);
			if ((g_NowStatus.WorkingBusy == USER_FREE) && (g_NowStatus.EmergencyStop == USER_FALSE))
				RingBuff_Insert(&g_DisPlayRingBuff, frame, rx_len);
		}
	}
	//重新开启DMA接收
	HAL_UARTEx_ReceiveToIdle_DMA(&LCD_UART,g_TFTDisplay.RxBuffer,bsp_TFT_USART_RX_BUFFER_SIZE);
}

