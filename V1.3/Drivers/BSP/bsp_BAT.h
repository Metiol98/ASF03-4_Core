#ifndef __BSP_BAT_H__
#define __BSP_BAT_H__

#include "main.h"

#define VOLTAGE_COEFF 7.666f

void BAT_Init(void);
void BAT_StartCollect(void);
float BAT_GetVoltage(void);

#endif



