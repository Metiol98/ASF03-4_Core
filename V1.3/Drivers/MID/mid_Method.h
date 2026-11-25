#ifndef __METHOD_H
#define __METHOD_H

#include "main.h"

#define SAVEMETHODSIZE	 				(sizeof(SaveMethodSTR))
#define METHOD_NAME_SIZE				60

extern volatile uint8_t g_MethodCount;  // 当前有效方法数量

void Method_Init(void);
void Flash_MethodLoad(void);
uint8_t Method_Add(char* name, uint8_t mode,uint32_t size);
uint8_t Method_Delete(uint8_t index);
void Method_DeletAll(void);
uint8_t Method_Export(uint8_t index);
void Method_ManageShow(uint8_t index);

#endif




