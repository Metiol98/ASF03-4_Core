#ifndef _BSP_TFT_DISPLAY_H
#define _BSP_TFT_DISPLAY_H
#include "HEAD.h"

#define 		RX_BUFF_LENGTH						30
/*******************************************************************************************/
#define			TFT_UART									USART1
#define			TFT_UART_CLK							RCC_APB2Periph_USART1
#define			TFT_UART_APBxClkCmd				RCC_APB2PeriphClockCmd		
#define			TFT_UART_BAUDRATE					115200

//USART GPIO引脚定义
#define			TFT_UART_GPIO_CLK					RCC_APB2Periph_GPIOA			//串口GPIO时钟
#define			TFT_UART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd		//串口GPIO时钟使能

#define			TFT_UART_TX_GPIO_PORT			GPIOA
#define			TFT_UART_TX_GPIO_PIN			GPIO_Pin_9
#define			TFT_UART_RX_GPIO_PORT			GPIOA
#define			TFT_UART_RX_GPIO_PIN			GPIO_Pin_10

//串口中断
#define			TFT_UART_IRQ							USART1_IRQn								//中断源的宏定义
#define			TFT_UART_IRQHandler				USART1_IRQHandler					//中断函数名的宏定义
/*******************************************************************************************/
extern uint8_t TFT_USART_RX_Finish;
extern uint8_t TFT_USART_CodeLength;//数据接收缓存器计数，全局使用
extern uint8_t TFT_USART_RX_BUF[RX_BUFF_LENGTH];//数据接收缓存器
extern uint8_t USART_LCD_RX_Counter;//数据接收缓存器计数,用于计数接收到多少个数据
extern uint8_t Code_Finnal_Counter1;//帧尾指针，用于接收帧尾计数
//屏幕初始化函数
static void TFT_UART_NVIC_Config(void);
static void TFT_UART_Config(void);
void TFT_Display_Init(void);
//屏幕功能函数
void SendWrokMode(u8 TFT_Screen_ID, u8 Control_ID, u8 WorkModeNum);
void DisplayControl(u8 TFT_Screen_ID, u8 Control_ID,u8 hide);
void SetTextValue(u8 TFT_Screen_ID,u16 TFT_CTL_ID,char *str);
void SwitchReSet(u8 TFT_Screen_ID, u8 Control_ID);
void SwitchSet(u8 TFT_Screen_ID, u8 Control_ID);
void SwitchScreen(u8 TFT_Screen_ID);
void StarTimer(u8 TFT_Screen_ID, u8 Control_ID);
void StopTimer(u8 TFT_Screen_ID, u8 Control_ID);
void ClearTimer(u8 TFT_Screen_ID, u8 Control_ID);
void ReadTextValue(u8 TFT_Screen_ID, u8 Control_ID);
void ReadTimer(u8 TFT_Screen_ID, u8 Control_ID);
void ScreenCalibration(void);
void SendTitleTextValue(uint32_t TitleFlag, u8 TFT_Screen_ID, u8 Control_ID);
void DataRecode_ClearRecode(u8 TFT_Screen_ID,u8 Control_ID);
void DataRecode_Add1Line(u8 TFT_Screen_ID,u8 Control_ID, char *str);
#endif

