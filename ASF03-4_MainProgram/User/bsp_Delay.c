#include "HEAD.h"


/*****************************************************************
* 名    称： RCC_Configuration()
* 功    能： 设置系统时钟72M
* 入口参数： 
* 出口参数： 
 *****************************************************************/
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;   /* 定义枚举类型变量 HSEStartUpStatus */	
	RCC_DeInit();      /* 复位系统时钟设置*/
	RCC_HSEConfig(RCC_HSE_ON);   	/* 开启HSE*/
	HSEStartUpStatus = RCC_WaitForHSEStartUp();	  /* 等待HSE起振并稳定*/
	if(HSEStartUpStatus == SUCCESS)    /* 判断HSE起是否振成功，是则进入if()内部 */
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1); /* 选择HCLK（AHB）时钟源为SYSCLK 1分频 */
		RCC_PCLK2Config(RCC_HCLK_Div1);  /* 选择PCLK2时钟源为 HCLK（AHB） 1分频 */
		RCC_PCLK1Config(RCC_HCLK_Div2);  /* 选择PCLK1时钟源为 HCLK（AHB） 2分频 */
		//FLASH_SetLatency(FLASH_Latency_2); /* 设置FLASH延时周期数为2 */
		//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); /* 使能FLASH预取缓存 */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); /* 选择锁相环（PLL）时钟源为HSE 1分频，倍频数为9，则PLL输出频率为 8MHz * 9 = 72MHz */
		RCC_PLLCmd(ENABLE);	                   /* 使能PLL */ 
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);   /* 等待PLL输出稳定 */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); /* 选择SYSCLK时钟源为PLL */
		while(RCC_GetSYSCLKSource() != 0x08);      /* 等待PLL成为SYSCLK时钟源 */
	} 
}

void Delay(__IO uint32_t nCount)
{
	for(;nCount!=0;nCount--);
}

void Systick_Delay_us(uint32_t us)
{
	uint32_t i;
	SysTick_Config(72);											//初始化定时器时钟为1us，最大不能超过重装载寄存器的值2的24次方
	
	for(i=0;i<us;i++)
	{
		while(!((SysTick->CTRL)&(1<<16)));
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭系统定时器
}

void Systick_Delay_ms(uint32_t ms)
{
	uint32_t i;
	SysTick_Config(72000);											//初始化定时器时钟为1ms
		
	for(i=0;i<ms;i++)
	{
		while(!((SysTick->CTRL)&(1<<16)));				//等待一个定时器周期结束，一个周期为1ms。
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭系统定时器
}

