#ifndef _BSP_BASICTIM_H_
#define _BSP_BASICTIM_H_
#include "HEAD.h"
/************************************************************************************/
//基本定时器7
#define Basic_TIME7									TIM7
#define Basic_TIME7_APBxClock_FUN		RCC_APB1PeriphClockCmd
#define Basic_TIME7_CLK							RCC_APB1Periph_TIM7	
#define Basic_TIME7_Period					(50000-1)							//自动重装载50ms
#define Basic_TIME7_Prescaler				(72-1)								//分频因子 
#define Basic_TIME7_IRQ							TIM7_IRQn							//中断源
#define Basic_TIME7_IRQHandler			TIM7_IRQHandler				//中断函数名

void BasicTIME7_NVIC(void);
void BasicTIME7_Config(void);
void BasicTIME7_Init(void);

#endif

