#include "bsp_BAT.h"

uint16_t Battery_voltage[10];    /* 电池电压值 */

/**
  * @brief  电池模块初始化
  * @param  None.
  * @retval None.
  */
void BAT_Init(void)
{
//  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Battery_voltage, 10);
  g_Battery.FirstPowerOn = USER_TRUE; /* 首次上电标志位置1，表示刚上电 */
	BAT_StartCollect();
}

/**
  * @brief  启动电池电压采集
  * @param  None.
  * @retval None.
  */
void BAT_StartCollect(void)
{
  memset(Battery_voltage, 0, sizeof(Battery_voltage));
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Battery_voltage, 10);
}

/**
  * @brief  获取电池电压值
  * @param  None.
  * @retval None.
  */
float BAT_GetVoltage(void)
{
  uint32_t sum = 0;
  float Battery_Voltage_AVG = 0.0f;
  for(int i = 0; i < 10; i++)
  {
      sum += Battery_voltage[i];
  }
  Battery_Voltage_AVG = (float)sum / 10.0f * (3.3f / 4095.0f) * VOLTAGE_COEFF; // Assuming a voltage divider with equal resistors
  return Battery_Voltage_AVG;
}

/**
  * @brief  ADC转换完成回调函数
  * @param  None.
  * @retval None.
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		HAL_ADC_Stop_DMA(&hadc1);

    g_Battery.AVG = BAT_GetVoltage();
    if(g_Battery.FirstPowerOn == USER_TRUE)
    {
      g_Battery.FirstPowerOn = USER_FALSE; /* 首次上电标志位置1，表示刚上电 */
      if(g_Battery.AVG < 20.66f)
			{
				TFTDisplay_DisplayControl(0,53,0);
				TFTDisplay_DisplayControl(0,52,0);
				TFTDisplay_DisplayControl(0,51,0);
				TFTDisplay_DisplayControl(0,50,1);
			}
			else if(g_Battery.AVG >= 20.66f && g_Battery.AVG < 22.18f)
			{
				TFTDisplay_DisplayControl(0,53,0);
				TFTDisplay_DisplayControl(0,52,0);
				TFTDisplay_DisplayControl(0,51,1);
				TFTDisplay_DisplayControl(0,50,0);
			}
			else if(g_Battery.AVG >= 22.18f && g_Battery.AVG < 23.69f)
			{
				TFTDisplay_DisplayControl(0,53,0);
				TFTDisplay_DisplayControl(0,52,1);
				TFTDisplay_DisplayControl(0,51,0);
				TFTDisplay_DisplayControl(0,50,0);
			}
			else if(g_Battery.AVG >= 23.69)
			{
				TFTDisplay_DisplayControl(0,53,1);
				TFTDisplay_DisplayControl(0,52,0);
				TFTDisplay_DisplayControl(0,51,0);
				TFTDisplay_DisplayControl(0,50,0);
			}
	  }
  }
}

/**
  * @brief  ADC错误回调函数
  * @param  None.
  * @retval None.
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  if(hadc->Instance == ADC1)
  {
    HAL_ADC_Stop_DMA(&hadc1);
  }
}























