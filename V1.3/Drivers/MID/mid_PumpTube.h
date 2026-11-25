#ifndef __MID_PUMPTUBE_H
#define __MID_PUMPTUBE_H

#include "main.h"

#define PUMPTUBE_CHANNEL_COUNT       4U
#define PUMPTUBE_STEPS_PER_ROTATION  51200ULL

typedef struct
{
	uint32_t replaced_at;
	uint64_t motor_steps;
	uint32_t cycle_months;
	uint32_t rotation_limit;
	uint8_t due_flags;
} PumpTubeChannelData;

typedef struct
{
	uint8_t enabled;
	PumpTubeChannelData channel[PUMPTUBE_CHANNEL_COUNT];
} PumpTubeData;

void PumpTube_Init(void);
void PumpTube_StartMotor(uint8_t channel);
void PumpTube_UpdatePosition(uint8_t channel, int32_t position, uint8_t busy);
void PumpTube_StopMotor(uint8_t channel, int32_t position);
void PumpTube_SetEnabled(uint8_t enabled);
uint8_t PumpTube_IsEnabled(void);
uint8_t PumpTube_GetDueMask(void);
void PumpTube_SetCycleMonths(uint8_t channel, uint32_t months);
void PumpTube_SetRotationLimit(uint8_t channel, uint32_t rotations);
void PumpTube_HandleManagementInput(uint16_t control, const uint8_t *frame, uint16_t length);
void PumpTube_OpenRegistration(void);
void PumpTube_HandleRegistration(uint16_t control, const uint8_t *frame, uint16_t length);
HAL_StatusTypeDef PumpTube_SaveNow(void);
void PumpTube_GetData(PumpTubeData *data);
void PumpTube_RefreshManagementPage(void);
void PumpTube_Service(void);

#endif
