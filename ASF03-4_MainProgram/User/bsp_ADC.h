#ifndef _BSP_ADC_H
#define _BSP_ADC_H
#include "HEAD.h"

/************************************************************************************/
//PowerEN相关GPIO
#define	PowerEN_GPIO_CLK						RCC_APB2Periph_GPIOB			//ADC_GPIO时钟
#define	PowerEN_GPIO_APBxClkCmd			RCC_APB2PeriphClockCmd		//ADC_GPIO时钟使能

#define	PowerEN_GPIO_PORT						GPIOB											//ADC引脚端口
#define	PowerEN_GPIO_PIN						GPIO_Pin_1								//ADC引脚

/************************************************************************************/
//ADC相关GPIO
#define	ADC_GPIO_CLK_A					RCC_APB2Periph_GPIOB			//ADC_GPIO时钟
#define	ADC_GPIO_APBxClkCmd			RCC_APB2PeriphClockCmd		//ADC_GPIO时钟使能

#define	ADC_V_IN1_GPIO_PORT			GPIOB											//ADC引脚端口
#define	ADC_V_IN1_GPIO_PIN			GPIO_Pin_0								//ADC引脚

//ADC编号，通道选择
#define	ADC_APBxClock_FUN				RCC_APB2PeriphClockCmd		//ADC使能函数
#define	ADC_V										ADC1											//ADC编号，ADC2不能使用DMA
#define	ADC_V_CLK								RCC_APB2Periph_ADC1				//ADC_RCC时钟

#define	ADC_V_IN1_Chanel				ADC_Channel_8							//ADC_RCC时钟

////ADC中断相关
//#define	ADC_IRQ												ADC1_2_IRQn								//ADC中断源，stm32f10x.h文件中找
//#define	ADC_IRQHandler								ADC1_2_IRQHandler					//ADC中断函数名，启动文件中找

//DMA相关
#define ADC_DMA_CLK  						RCC_AHBPeriph_DMA1			//根据手册查询ADC1要对应DMA和DMA通道
#define ADC_DMA_CHANEL 					DMA1_Channel1

#define ADC_ConvertedValueSize  4			//缓冲区大小
#define Sample_Num  						20


extern uint16_t ADC_ConvertedValue[Sample_Num];	//ADC数据缓存区
extern uint32_t ADC_Value_SUM;									//采样数据平均滤波临时总和
extern float 		ADC_Value;											//ADC滤波后的值
extern float 		Battery_V;											//转换后电压值

extern uint8_t 	PowerEN_Flag;//
extern uint8_t 	PowerEN_Blink;									//充电时的电量动图标志计数

void ADCx_Mode_Config(void);
void ADC_DMA_Cofig(void);
void ADC_DMA_Init(void);
void BasicTIME7_Config(void);
#endif
