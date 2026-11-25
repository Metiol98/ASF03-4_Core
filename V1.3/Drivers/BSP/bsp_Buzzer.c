#include "bsp_Buzzer.h"

void buzzer_init(void)
{
	
}

/**
  * @brief  ????250ms
  */
void Beep_S(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
	SysTickDelay_ms(250);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}

/**
  * @brief  ????500ms
  */
void Beep_M(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
	SysTickDelay_ms(500);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}

/**
  * @brief  ????1000ms
  */
void Beep_L(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
	SysTickDelay_ms(1000);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}

/**
  * @brief  ??????
  * @param  Delayms	????
  * @param  Times		????
  * @retval None
  */
void NoticeBeep(uint32_t Delayms, uint32_t Times)
{
	for (int i = 0; i < Times; i++)
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
		SysTickDelay_ms(Delayms);
		HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
		SysTickDelay_ms(Delayms);
	}
}
