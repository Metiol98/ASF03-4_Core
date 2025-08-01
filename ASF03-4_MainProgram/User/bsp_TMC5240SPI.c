#include "HEAD.h"
/*
typedef struct
{
  uint16_t SPI_Direction;       	//SPI单双向模式
  uint16_t SPI_Mode;    					//SPI主从机模式
  uint16_t SPI_DataSize;       		//SPI的数据帧长度，可选8/16
  uint16_t SPI_CPOL;          		//时钟极性，可选高/低电平
  uint16_t SPI_CPHA;       				//时钟相位，可选奇/偶边缘采样
  uint16_t SPI_NSS;              	//片选脚由软件控制还是硬件控制
  uint16_t SPI_BaudRatePrescaler; //设置时钟分频因子
  uint16_t SPI_FirstBit;    			//设置MSB/LSB先行，高位先传还是低位先传
  uint16_t SPI_CRCPolynomial;    	//设置CRC校验的表达式
}SPI_InitTypeDef;

*/

void Motor_SPI_Init(void)
{
	SPI_InitTypeDef		SPI_InitStructur;
	GPIO_InitTypeDef	GPIO_InitStructur;	
	
	//使能SPI时钟
	Motor_SPI_APBxClkCmd(Motor_SPI_CLK, ENABLE);
	//使能SPI的GPIO时钟
	Motor_SPI_SCK_APBxClkCmd(Motor_SPI_SCK_CLK|
													Motor_SPI_MISO_CLK|
													Motor_SPI_MOSI_CLK|
													Motor_SPI_CS1_CLK, ENABLE);

	//SCK，推挽复用
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_SCK_PIN;
	GPIO_InitStructur.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(Motor_SPI_SCK_PORT,&GPIO_InitStructur);
	
	//MISO， 
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_MISO_PIN;
	GPIO_Init(Motor_SPI_MISO_PORT,&GPIO_InitStructur);
	
	//MOSI，推挽复用
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_MOSI_PIN;
	GPIO_Init(Motor_SPI_MOSI_PORT,&GPIO_InitStructur);
	
	//CS1
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_CS1_PIN;
	GPIO_InitStructur.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(Motor_SPI_CS1_PORT,&GPIO_InitStructur);
	//CS2
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_CS2_PIN;
	GPIO_Init(Motor_SPI_CS2_PORT,&GPIO_InitStructur);
	//CS3
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_CS3_PIN;
	GPIO_Init(Motor_SPI_CS3_PORT,&GPIO_InitStructur);
	//CS4
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_CS4_PIN;
	GPIO_Init(Motor_SPI_CS4_PORT,&GPIO_InitStructur);
	//EN1
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_EN1_PIN;
	GPIO_Init(Motor_SPI_EN1_PORT,&GPIO_InitStructur);
	//EN2
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_EN2_PIN;
	GPIO_Init(Motor_SPI_EN2_PORT,&GPIO_InitStructur);
	//EN3
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_EN3_PIN;
	GPIO_Init(Motor_SPI_EN3_PORT,&GPIO_InitStructur);
	//EN4
	GPIO_InitStructur.GPIO_Pin=Motor_SPI_EN4_PIN;
	GPIO_Init(Motor_SPI_EN4_PORT,&GPIO_InitStructur);
	
	
	//【SPI初始化】
	SPI_InitStructur.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructur.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructur.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructur.SPI_CPOL=SPI_CPOL_High;
	SPI_InitStructur.SPI_CPHA=SPI_CPHA_2Edge;
	SPI_InitStructur.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStructur.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_64;		//4分频
	SPI_InitStructur.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructur.SPI_CRCPolynomial=7;				//不适用CRC，随便赋值
	
	SPI_Init(Motor_SPIx, &SPI_InitStructur);
	SPI_Cmd(Motor_SPIx, ENABLE);
	
	Motor_SPI_CS1_OFF;
	Motor_SPI_CS2_OFF;
	Motor_SPI_CS3_OFF;
	Motor_SPI_CS4_OFF;
	
	Motor_SPI_EN1_OFF;
	Motor_SPI_EN2_OFF;
	Motor_SPI_EN3_OFF;
	Motor_SPI_EN4_OFF;
}


/*******************************************************************************************
*******************************************************************************************
*******************************************************************************************
*******************************************************************************************/

/*********************************************************
函数名称：SPI_Flash_SendByte(uint8_t byte)
描    述：
//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
形    参：byte：要发送的字节
*********************************************************/
void SPI_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
} 

/*********************************************************
函数名称：SPI_Flash_SendByte(uint8_t byte)
描    述：发送一个字节，SPI在发送的同时也会接收。要发送时必须接收，要接收是必须发送。
形    参：byte：要发送的字节
*********************************************************/
uint8_t Motor_SPI_ReadWriteByte(uint8_t byte)
{
	uint32_t SPI_TIMEout;
	
	SPI_TIMEout=Motor_SPI_FLAG_TimeOut;
	while(SPI_I2S_GetFlagStatus(Motor_SPIx, SPI_I2S_FLAG_TXE)==0)//检查并检查TX缓冲区为空。
	{
		if((SPI_TIMEout--)==0)
		{
			SPI_TIMEout=Motor_SPI_FLAG_TimeOut;
			return 0;
		}
	}
	
	SPI_I2S_SendData(Motor_SPIx, byte);
	
	SPI_TIMEout=Motor_SPI_FLAG_TimeOut;
	while(SPI_I2S_GetFlagStatus(Motor_SPIx, SPI_I2S_FLAG_RXNE)==0)//检查并检查RX接收数据，如果接收到数据表示发送完成。
	{
		if((SPI_TIMEout--)==0)
			return 0;
	}
	
	return SPI_I2S_ReceiveData(Motor_SPIx);   //返回接收到的数据
}

/*********************************************************
函数名称：SPI_Flash_ReadByte(void)
描    述：读取一个字节。要发送时必须接收，要接收时必须发送，所以单独的接收程序没有意义。
形    参：
*********************************************************/
uint8_t SPI_Flash_ReadByte(void)
{
	return Motor_SPI_ReadWriteByte(DUMMY); 
}

/*******************************************************************************************
*******************************************************************************************
*******************************************************************************************
*******************************************************************************************/

/**********************************************END FILE**********************************************/
