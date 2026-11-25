#ifndef __MID_UART_H
#define __MID_UART_H

#include "main.h"

// 打印缓冲区大小（按需调整，默认128足够）
#define UART1_PRINTF_BUF_SIZE 128
#define UART5_PRINTF_BUF_SIZE 128

uint16_t WIFI_printf(const char *fmt, ...);
uint16_t Debug_printf(const char *fmt, ...);

void TFT_DecodeCommand(uint8_t *arr);


#endif








