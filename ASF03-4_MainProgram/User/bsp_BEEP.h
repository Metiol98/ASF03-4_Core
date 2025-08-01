#ifndef _BSP_BEEP_H
#define _BSP_BEEP_H
#include "HEAD.h"

/********************************************/
//串口GPIO相关
#define	BEEP_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd		//ADC_GPIO时钟使能
#define	BEEP_GPIO_CLK					RCC_APB2Periph_GPIOA			//ADC_GPIO时钟

#define BEEP_GPIO_PORT				GPIOA
#define BEEP_GPIO_PIN					GPIO_Pin_0

#define BEEP_ON								GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_OFF							GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)


void BEEP_Init(void);	//初始化
void Beep_S(void);   //蜂鸣器快速响，间隔250ms
void Beep_M(void);   //蜂鸣器快速响，间隔500ms
void Beep_L(void);   //蜂鸣器快速响，间隔1000ms		 		

#endif

