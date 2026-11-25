#ifndef __BSP_RINGQUEUE_H
#define __BSP_RINGQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* -------------------宏定义--------------------------- */
#define RingBuff_TRUE						1								/* 真 */
#define RingBuff_FALSE					(!RingBuff_TRUE)		/* 假 */

#define RINTQUEUESIZE								  		120		/* 数组大小 */

/* -------------------变量声明------------------------- */
/* 环形队列结构体 */
typedef struct
{
	uint8_t   array[RINTQUEUESIZE]; /* 环形队列数组 */
	uint16_t  front;    /* 指向队列的头部 */
	uint16_t  rear;     /* 指向队列的尾部 */
  uint16_t  size;       /* 队列的大小 */
}RingBuffTypeDef;

extern RingBuffTypeDef g_DisPlayRingBuff;		/* 声明环形队列结构体 */

/* 环形队列初始化函数 */
void RingBuff_Init(RingBuffTypeDef *p);
uint8_t RingBuff_GetFFullstate(RingBuffTypeDef *p);
uint8_t RingBuff_GetEmptystate(RingBuffTypeDef *p);
uint8_t RingBuff_Insert(RingBuffTypeDef *p,uint8_t *Data,uint32_t len);
uint8_t RingBuff_Getout(RingBuffTypeDef *p,uint8_t *Data,uint32_t *len);


#endif






