#ifndef __MID_PUMP_H
#define __MID_PUMP_H

#include "main.h"

/* 枚举变量 */
typedef enum
{
	CW = 0,
	CCW =1
} MotorDIR;

/* 枚举变量 */
typedef enum
{
	Wait = 1,
	NoWait
} MotorWait;

void PumpMotion(uint8_t Channel,MotorDIR Dir,float Volume,MotorWait Waits);
void PumpRotate(uint8_t Channel,MotorDIR Dir,MotorWait Waits);
void PumpStop(uint8_t Channel);

#endif

