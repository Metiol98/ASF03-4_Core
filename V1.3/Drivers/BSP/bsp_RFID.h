#ifndef __BSP_RFID_H
#define __BSP_RFID_H

#include "main.h"

#define RFID_UART            huart3
// #define RFID_DMA_UART_RX     hdma_usart3_rx

#define RfidRxBufferSize   15     /* ?????? */

// SRX885??????
typedef enum {
    SRX885_PAIR_UNKNOWN = 0,
    SRX885_PAIR_SUCCESS,  // ????
    SRX885_PAIR_TIMEOUT,  // ????
    SRX885_PAIR_FAILED    // ????
} SRX885_Pair_Status;

void RFID_init(void);
void RFID_IRQ_Process(void);
//uint8_t Data_Right_Handler(void)£»

void RFID_Mode_DoubleClick(void);
void RFID_Mode_ClearAll(void);
void RFID_Mode_PairMode(void);

#endif

