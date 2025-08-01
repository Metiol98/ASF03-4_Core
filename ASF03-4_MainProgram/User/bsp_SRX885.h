#ifndef _BSP_SRX885_H
#define _BSP_SRX885_H

extern uint8_t 	SRX885_Button_1;
extern uint8_t 	SRX885_Button_2;
extern uint32_t SRX885Counter;//接收计数,防止按键粘连
extern uint8_t 	SRX885_Finish;//
/*******************************************************************************************/
#define			SRX885_UART									USART3
#define			SRX885_UART_CLK							RCC_APB1Periph_USART3
#define			SRX885_UART_APBxClkCmd			RCC_APB1PeriphClockCmd		
#define			SRX885_UART_BAUDRATE				9600

//USART GPIO引脚定义
#define			SRX885_UART_GPIO_CLK				RCC_APB2Periph_GPIOB			//串口GPIO时钟
#define			SRX885_UART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd		//串口GPIO时钟使能

//#define			SRX885_UART_TX_GPIO_PORT		GPIOA
//#define			SRX885_UART_TX_GPIO_PIN			GPIO_Pin_2
//USART GPIO引脚定义PB11
#define			SRX885_UART_RX_GPIO_PORT		GPIOB
#define			SRX885_UART_RX_GPIO_PIN			GPIO_Pin_11

//串口中断
#define			SRX885_UART_IRQ							USART3_IRQn								//中断源的宏定义
#define			SRX885_UART_IRQHandler			USART3_IRQHandler					//中断函数名的宏定义

extern uint8_t WorkBusy;//0表示空闲，1表示忙碌
static void SRX885_UART_NVIC_Config(void);
static void SRX885_UART_Config(void);
void SRX885_UART_Init(void);
#endif
