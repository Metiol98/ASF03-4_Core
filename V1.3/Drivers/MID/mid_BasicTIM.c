#include "mid_basicTIM.h"


/**
  * @brief  定时器优先级初始化
  * @param  None
  * @retval None
  */
 void TIM_Init(void)
 {
    HAL_TIM_Base_Start_IT(&htim7); //启动定时器2中断
 }


uint8_t TIM7_Motor_CNT = 0;
/**
  * @brief  TIM7中断函数
  * @note   50ms触发一次
  * @param  None.
  * @retval None.
  */
void TIM7_IRQ_Process(void)
{
  /* 电机状态查询 */
  TIM7_Motor_CNT++;			/* 电机查询次数 */
	g_Battery.TimCNT++;		/* 电池定时器计数 */
  
  if(TIM7_Motor_CNT > 4)
  {//每200ms采集轮询一次数据
    TIM7_Motor_CNT = 0;

    if(Channel1_MotorStatus.Busy == USER_BUSY)
    {
      TMC5240_QueryMotorStatus(1);
      PumpTube_UpdatePosition(1, (int32_t)TMC5240_ReadRegister(1, TMC5240_XACTUAL), Channel1_MotorStatus.Busy);
    }

    if(Channel2_MotorStatus.Busy == USER_BUSY)
    {
      TMC5240_QueryMotorStatus(2);
      PumpTube_UpdatePosition(2, (int32_t)TMC5240_ReadRegister(2, TMC5240_XACTUAL), Channel2_MotorStatus.Busy);
    }

    if(Channel3_MotorStatus.Busy == USER_BUSY)
    {
      TMC5240_QueryMotorStatus(3);
      PumpTube_UpdatePosition(3, (int32_t)TMC5240_ReadRegister(3, TMC5240_XACTUAL), Channel3_MotorStatus.Busy);
    }

    if(Channel4_MotorStatus.Busy == USER_BUSY)
    {
      TMC5240_QueryMotorStatus(4);
      PumpTube_UpdatePosition(4, (int32_t)TMC5240_ReadRegister(4, TMC5240_XACTUAL), Channel4_MotorStatus.Busy);
    }
  }

	/* 循环模式，间隔计数 */
	if((WorkMode == 2) && (g_NowStatus.WorkingBusy == USER_BUSY))
		g_Interval.InterTimeCNT++;
	if(g_Interval.InterTimeCNT >= g_Interval.IntervalTime*20)
	{
		g_Interval.InterTimeCNT = 0;
		g_Interval.InterTimeOver = USER_TRUE;//循环模式间隔时间到时间到！
	}
	
  /* 手柄按键防止粘黏 */
  g_NowStatus.SRX885Counter++;         /* 计数器计数，防止按键粘连重复进入程序 */
  if(g_NowStatus.SRX885Counter>10)     /* 按键时间间隔必须大约500ms才有效，不然视为无效 */
  {
    g_NowStatus.SRX885Counter = 50;    /* 防止超限制 */
    g_NowStatus.SRX885OK = USER_TRUE;  /* 连续按键之间有足够时间间隔，没有粘黏（按住不放） */
  }

  /* 电池定时器计数 */
  if(g_Battery.TimCNT >= 400)  /* 防止超限制 */
  {
    g_Battery.TimCNT = 0;                /* 重新计时 */
  }

  /* 电池状态指示 -- 电池电量指示 */
  if(g_Battery.TimCNT % 10 == 0)
  {
		BAT_StartCollect();

    /* 电池电量指示 */
    if(HAL_GPIO_ReadPin(POWER_EN_GPIO_Port,POWER_EN_Pin) != g_Battery.OutPowerFlag)
			g_Battery.OutPowerFlag = HAL_GPIO_ReadPin(POWER_EN_GPIO_Port,POWER_EN_Pin);
    if(g_Battery.OutPowerFlag == USER_TRUE)
      TFTDisplay_DisplayControl(0,16,USER_DISPLAY);/* 充电器接入指示，显示电池充电图标 */
    else
      TFTDisplay_DisplayControl(0,16,USER_HIDE);/* 隐藏电池充电图标 */

    g_Battery.IconBlink = !g_Battery.IconBlink;/* 图标闪烁状态切换 */
    if(g_Battery.AVG < 19.07f && g_Battery.OutPowerFlag == USER_FALSE)/* 电池电量指示，低于19.07V时闪烁 */
    {
      if(g_Battery.IconBlink == USER_TRUE)
        TFTDisplay_DisplayControl(0,50,1);
      if(g_Battery.IconBlink == USER_FALSE)
        TFTDisplay_DisplayControl(0,50,0);
      
      TFTDisplay_DisplayControl(0,53,0);
			TFTDisplay_DisplayControl(0,52,0);
			TFTDisplay_DisplayControl(0,51,0);
    }

    if(g_Battery.OutPowerFlag == USER_TRUE)
    {
      TFTDisplay_DisplayControl(0,16,1);/* 显示电池电量图标 */

      g_Battery.IconBlinkCNT++;
      if(g_Battery.IconBlinkCNT == 4)
      {
        if(g_Battery.AVG < 20.58f)
          g_Battery.IconBlinkCNT = 0;
        else if(g_Battery.AVG >= 20.58f && g_Battery.AVG < 22.09f)
          g_Battery.IconBlinkCNT = 1;
        else if(g_Battery.AVG >= 22.09f && g_Battery.AVG < 23.6f)
          g_Battery.IconBlinkCNT = 2;
        else if(g_Battery.AVG >= 23.6f)
          g_Battery.IconBlinkCNT = 3;
      }
      switch (g_Battery.IconBlinkCNT)
      {
        case 0:
          TFTDisplay_DisplayControl(0,53,0);
          TFTDisplay_DisplayControl(0,52,0);
          TFTDisplay_DisplayControl(0,51,0);
          TFTDisplay_DisplayControl(0,50,1);
          break;

        case 1:
          TFTDisplay_DisplayControl(0,53,0);
          TFTDisplay_DisplayControl(0,52,0);
          TFTDisplay_DisplayControl(0,51,1);
          TFTDisplay_DisplayControl(0,50,0);
        break;
      
        case 2:
          TFTDisplay_DisplayControl(0,53,0);
          TFTDisplay_DisplayControl(0,52,1);
          TFTDisplay_DisplayControl(0,51,0);
          TFTDisplay_DisplayControl(0,50,0);
          break;

        case 3:
          TFTDisplay_DisplayControl(0,53,1);
          TFTDisplay_DisplayControl(0,52,0);
          TFTDisplay_DisplayControl(0,51,0);
          TFTDisplay_DisplayControl(0,50,0);
          break;
        
        default:
          break;
      }
    }
  }

  /* 电池状态指示 -- 电池电量指示 */
  if((g_Battery.TimCNT % 50 == 0) && (g_Battery.OutPowerFlag == USER_FALSE) && (g_Battery.AVG > 19.07f))  /* 2500ms处理一次 */
  {
    if(g_Battery.AVG < 20.58f && (g_Battery.OutPowerFlag == USER_FALSE))
    {
      TFTDisplay_DisplayControl(0,53,0);
      TFTDisplay_DisplayControl(0,52,0);
      TFTDisplay_DisplayControl(0,51,0);
      TFTDisplay_DisplayControl(0,50,1);
    }
    else if(g_Battery.AVG >= 20.58f && g_Battery.AVG < 22.09f && (g_Battery.OutPowerFlag == USER_FALSE))
    {
      TFTDisplay_DisplayControl(0,53,0);
      TFTDisplay_DisplayControl(0,52,0);
      TFTDisplay_DisplayControl(0,51,1);
      TFTDisplay_DisplayControl(0,50,0);
    }
    else if(g_Battery.AVG >= 22.09f && g_Battery.AVG < 23.6f && (g_Battery.OutPowerFlag == USER_FALSE))
    {
      TFTDisplay_DisplayControl(0,53,0);
      TFTDisplay_DisplayControl(0,52,1);
      TFTDisplay_DisplayControl(0,51,0);
      TFTDisplay_DisplayControl(0,50,0);
    }
    else if(g_Battery.AVG >= 23.6 && (g_Battery.OutPowerFlag == USER_FALSE))
    {
      TFTDisplay_DisplayControl(0,53,1);
      TFTDisplay_DisplayControl(0,52,0);
      TFTDisplay_DisplayControl(0,51,0);
      TFTDisplay_DisplayControl(0,50,0);
    }
  }
}

/**
  * @brief  TIM定时器回调函数
  * @param  None.
  * @retval None.
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM7)
  {
    TIM7_IRQ_Process();
  }
}













