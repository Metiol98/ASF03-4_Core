#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H

#include "main.h"

void buzzer_init(void);

void Beep_S(void);
void Beep_M(void);
void Beep_L(void);
void NoticeBeep(uint32_t Delayms, uint32_t Times);

#endif

