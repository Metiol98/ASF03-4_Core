#ifndef _BSP_DELAY_H
#define _BSP_DELAY_H
#include "HEAD.h"

#define	SOFT_Delay				Delay(0x4FFFFF);
#define	SOFT_ShortDelay		Delay(0x1FFFFF);
#define	SOFT_LongDelay		Delay(0x4FFFFFF);
#define LED_Delay					Systick_Delay_ms(100)

void RCC_Configuration(void);
void Delay(__IO uint32_t nCount);
void Systick_Delay_us(uint32_t us);
void Systick_Delay_ms(uint32_t ms);


#endif

