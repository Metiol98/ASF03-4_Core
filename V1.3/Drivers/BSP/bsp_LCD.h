#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include "main.h"

#define LCD_UART            huart1
#define LCD_DMA_UART_RX     hdma_usart1_rx

#define LCD_CMD_HEAD  0XEE
#define LCD_CMD_TAIL  0XFFFCFFFF

// 定义环形缓冲区
#define RingBufferSize 			120

typedef struct {
    uint8_t buffer[RingBufferSize];  // 缓冲区指针
    uint16_t r;        							 // 读指针
    uint16_t w;        						 	 // 写指针
    uint16_t itemCount;        			 // 当前缓冲区中存储的数据个数
} RingBufferDef;

#define     bsp_TFT_USART_RX_BUFFER_SIZE      	60  /* TFT显示屏串口接收缓冲区大小 */

typedef struct
{	
  uint8_t RxFinish;
  uint8_t RxBuffer[bsp_TFT_USART_RX_BUFFER_SIZE];
} TFTDisplaySTR;
extern TFTDisplaySTR  g_TFTDisplay;

void LCD_init(void);
uint8_t LCD_CheckData(uint8_t *data,uint8_t len);
void LCD_IRQ_Process(void);

int TFTDisplay_ASCIItoDEC(uint8_t *arry);
float TFTDisplay_ASCIItoFloat(uint8_t *Arry);

void TFTDisplay_DisplayControl(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID,uint8_t hide);
void TFTDisplay_SetTextValue(uint16_t TFT_Screen_ID,uint16_t TFT_Control_ID,char *str);
void TFTDisplay_SwitchControl(uint16_t TFT_Screen_ID,uint16_t TFT_Control_ID,uint8_t sta);
void TFTDisplay_SwitchScreen(uint8_t TFT_Screen_ID);
void TFTDisplay_ReadTextValue(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID);
void TFTDisplay_ClearTextValue(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID);
void TFTDisplay_StarTimer(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID);
void TFTDisplay_StopTimer(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID);
void TFTDisplay_ReadTimer(uint16_t TFT_Screen_ID, uint16_t TFT_Control_ID);
typedef struct
{
	uint8_t year; /* Years since 2000, as returned by the LCD. */
	uint8_t month, day, hour, minute, second;
} TFTDateTime;

void TFTDisplay_RequestRTC(void);
uint8_t TFTDisplay_ReadRTC(TFTDateTime *date, uint32_t *received_at);
void TFTDisplay_DataRecodeClearRecode(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID);
void TFTDisplay_DataRecodeAddLine(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID, char *str);
void TFTDisplay_DataRecodeDisplayControl(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID,uint8_t hide);
void TFTDisplay_Slider(uint8_t TFT_Screen_ID,uint8_t TFT_Control_ID, uint8_t value);
void TFTDisplay_CurveControl(uint8_t TFT_Screen_ID,uint8_t Control_ID, float value, float Scale);
void TFTDisplay_CurveSet(uint8_t TFT_Screen_ID,uint8_t Control_ID, int8_t HorOffset, uint32_t HorScal, int8_t VerOffset, uint32_t VerScal);
void TFTDisplay_SendTitleTextValue(uint8_t TFT_Screen_ID, uint8_t Control_ID, uint32_t TitleFlag);


#endif

