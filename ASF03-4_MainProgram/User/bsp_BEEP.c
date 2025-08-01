#include "HEAD.h"

void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	BEEP_GPIO_APBxClkCmd(BEEP_GPIO_CLK, ENABLE);	 //使能GPIOB端口时钟

	//PA0
	GPIO_InitStructure.GPIO_Pin=BEEP_GPIO_PIN;  			//初始化 GPIOA的引脚为推挽输出*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		//A8为蜂鸣器BUZZER引脚
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	 
	GPIO_Init(BEEP_GPIO_PORT,&GPIO_InitStructure);

	BEEP_OFF;	//输出0，关闭蜂鸣器输出       
}

void Beep_S(void)   //蜂鸣器快速响，间隔250ms
{ 
	BEEP_ON;   //蜂鸣器响
	Systick_Delay_ms(250);
	BEEP_OFF;     //蜂鸣器不响
}	
void Beep_M(void)  //蜂鸣器中速响，间隔500ms
{ 
	BEEP_ON;   //蜂鸣器响
	Systick_Delay_ms(500);
	BEEP_OFF;     //蜂鸣器不响						
}	
void Beep_L(void) //蜂鸣器慢速响，间隔1000ms
{ 
	BEEP_ON;   //蜂鸣器响
	Systick_Delay_ms(1000);
	BEEP_OFF;     //蜂鸣器不响?			
}
