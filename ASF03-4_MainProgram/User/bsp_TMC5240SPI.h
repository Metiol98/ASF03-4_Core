#ifndef _BSP_TMC5240SPI_H
#define _BSP_TMC5240SPI_H
#include "HEAD.h"

/*SPI接口相关定义*/
#define		Motor_SPIx									SPI1
#define	  Motor_SPI_APBxClkCmd				RCC_APB2PeriphClockCmd 
#define		Motor_SPI_CLK								RCC_APB2Periph_SPI1

//SCK引脚 PA5
#define		Motor_SPI_SCK_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_SCK_CLK						RCC_APB2Periph_GPIOA
#define		Motor_SPI_SCK_PORT					GPIOA
#define		Motor_SPI_SCK_PIN						GPIO_Pin_5
	
//MISO引脚 PA6
#define		Motor_SPI_MISO_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_MISO_CLK					RCC_APB2Periph_GPIOA
#define		Motor_SPI_MISO_PORT					GPIOA
#define		Motor_SPI_MISO_PIN					GPIO_Pin_6

//MOSI引脚 PA7
#define		Motor_SPI_MOSI_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_MOSI_CLK					RCC_APB2Periph_GPIOA
#define		Motor_SPI_MOSI_PORT					GPIOA
#define		Motor_SPI_MOSI_PIN					GPIO_Pin_7

/********************************************************************/
//CS1 PC0
#define		Motor_SPI_CS1_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_CS1_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_CS1_PORT					GPIOC
#define		Motor_SPI_CS1_PIN						GPIO_Pin_0
//CS2 PC1
#define		Motor_SPI_CS2_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_CS2_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_CS2_PORT					GPIOC
#define		Motor_SPI_CS2_PIN						GPIO_Pin_1

//CS3 PC2
#define		Motor_SPI_CS3_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_CS3_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_CS3_PORT					GPIOC
#define		Motor_SPI_CS3_PIN						GPIO_Pin_2

//CS4 PC3
#define		Motor_SPI_CS4_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_CS4_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_CS4_PORT					GPIOC
#define		Motor_SPI_CS4_PIN						GPIO_Pin_3

//EN1 PC4
#define		Motor_SPI_EN1_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_EN1_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_EN1_PORT					GPIOC
#define		Motor_SPI_EN1_PIN						GPIO_Pin_4
//EN2 PC5
#define		Motor_SPI_EN2_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_EN2_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_EN2_PORT					GPIOC
#define		Motor_SPI_EN2_PIN						GPIO_Pin_5

//EN3 PC6
#define		Motor_SPI_EN3_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_EN3_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_EN3_PORT					GPIOC
#define		Motor_SPI_EN3_PIN						GPIO_Pin_6

//EN4 PC7
#define		Motor_SPI_EN4_APBxClkCmd		RCC_APB2PeriphClockCmd
#define		Motor_SPI_EN4_CLK						RCC_APB2Periph_GPIOC
#define		Motor_SPI_EN4_PORT					GPIOC
#define		Motor_SPI_EN4_PIN						GPIO_Pin_7


//SPI片选控制引脚
#define 	Motor_SPI_CS1_OFF						{GPIO_SetBits(Motor_SPI_CS1_PORT,Motor_SPI_CS1_PIN);}
#define 	Motor_SPI_CS1_ON						{GPIO_ResetBits(Motor_SPI_CS1_PORT,Motor_SPI_CS1_PIN);}																

#define 	Motor_SPI_CS2_OFF						{GPIO_SetBits(Motor_SPI_CS2_PORT,Motor_SPI_CS2_PIN);}
#define 	Motor_SPI_CS2_ON						{GPIO_ResetBits(Motor_SPI_CS2_PORT,Motor_SPI_CS2_PIN);}																	

#define 	Motor_SPI_CS3_OFF						{GPIO_SetBits(Motor_SPI_CS3_PORT,Motor_SPI_CS3_PIN);}
#define 	Motor_SPI_CS3_ON						{GPIO_ResetBits(Motor_SPI_CS3_PORT,Motor_SPI_CS3_PIN);}																	

#define 	Motor_SPI_CS4_OFF						{GPIO_SetBits(Motor_SPI_CS4_PORT,Motor_SPI_CS4_PIN);}
#define 	Motor_SPI_CS4_ON						{GPIO_ResetBits(Motor_SPI_CS4_PORT,Motor_SPI_CS4_PIN);}																	

//EN片选控制引脚
#define 	Motor_SPI_EN1_OFF						{GPIO_SetBits(Motor_SPI_EN1_PORT,Motor_SPI_EN1_PIN);}
#define 	Motor_SPI_EN1_ON						{GPIO_ResetBits(Motor_SPI_EN1_PORT,Motor_SPI_EN1_PIN);}																

#define 	Motor_SPI_EN2_OFF						{GPIO_SetBits(Motor_SPI_EN2_PORT,Motor_SPI_EN2_PIN);}
#define 	Motor_SPI_EN2_ON						{GPIO_ResetBits(Motor_SPI_EN2_PORT,Motor_SPI_EN2_PIN);}																	

#define 	Motor_SPI_EN3_OFF						{GPIO_SetBits(Motor_SPI_EN3_PORT,Motor_SPI_EN3_PIN);}
#define 	Motor_SPI_EN3_ON						{GPIO_ResetBits(Motor_SPI_EN3_PORT,Motor_SPI_EN3_PIN);}																	

#define 	Motor_SPI_EN4_OFF						{GPIO_SetBits(Motor_SPI_EN4_PORT,Motor_SPI_EN4_PIN);}
#define 	Motor_SPI_EN4_ON						{GPIO_ResetBits(Motor_SPI_EN4_PORT,Motor_SPI_EN4_PIN);}																	
/********************************************************************/

#define		Motor_SPI_FLAG_TimeOut			((uint32_t)0x1000)
#define		Motor_SPI_LONG_TimeOut			((uint32_t)(10*I2C_FLAG_TimeOut))

/********************************************************************/
//指令
#define		DUMMY												0x00	//空闲发送无效数据

/********************************************************************/
//TMC5240SPI功能初始化函数
void Motor_SPI_Init(void);

//TMC5240SPI功能接发函数
uint8_t Motor_SPI_ReadWriteByte(uint8_t byte);//发送一个字节
uint8_t SPI_Flash_ReadByte(void);							//接收一个字节

//TMC5240功能函数

#endif

