#include "mid_Delay.h"

/**
  * @brief  SysTick微秒级延时,是阻塞延时
  * @param  nus：为要延时的us数.
  * @retval None
  */
void SysTickDelay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;  // SysTick重装载寄存器的值是SystemCoreClock/1000 - 1，表示1ms
    ticks = us * (SystemCoreClock / 1000000);  // 计算出需要的计数值，1us = SystemCoreClock/1e6 计数值

    told = SysTick->VAL;  // 记录当前SysTick计数值
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            // SysTick是一个递减计数器，判断当前值是否小于上一个值
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;
            
            told = tnow;
            if (tcnt >= ticks)
                break;  // 超过延时，退出循环
        }
    }
}


/**
  * @brief  SysTick毫秒级延时,是阻塞延时
  * @param  ms: 为要延时的毫秒数
  * @retval None
  */
void SysTickDelay_ms(uint32_t ms)
{
	#if 1
	for(uint32_t i=0;i<ms;i++) 
		SysTickDelay_us(1000);
	#endif
	
	#if 0	/*	使用Systick延时函数时，可能会导致Systick延时函数失效	*/
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;  // 获取SysTick重装载寄存器的值是SystemCoreClock/1000 - 1，表示1ms

	// 计算出需要的计数值，1ms = SystemCoreClock/1000 计数值
	ticks = ms * (SystemCoreClock / 1000);

	told = SysTick->VAL;  // 记录当前SysTick计数值
	while (1)
	{
			tnow = SysTick->VAL;
			if (tnow != told)
			{
					// 处理SysTick计数值变化，判断当前值是否小于上一个值
					if (tnow < told)
							tcnt += told - tnow;
					else
							tcnt += reload - tnow + told;
					
					told = tnow;
					if (tcnt >= ticks)
							break;  // 超过延时，退出循环
			}
	}
	#endif
}







