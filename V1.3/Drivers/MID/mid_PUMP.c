#include "mid_PUMP.h"

/**
  * @brief  蠕动泵动作
  * @param  Dir:    正传或反转，加注或回吸
  * @param  Volume: 加液体积，以单步加液量为准,单位mL
  * @param  Waits:  是否需要等待，死等
  * @retval None
  */
void PumpMotion(uint8_t Channel,MotorDIR Dir,float Volume,MotorWait Waits)
{
	if ((Channel < 1U) || (Channel > 4U))
		return;
	TMC5240_WriteReadRegister(Channel,TMC5240_VMAX,g_MCBConfig.PumpSpeed[Channel-1]*1193,TMC_WRITE);
	
	//固定计算参数
	const double  SCALE_1 = 1000.0;		//转换成uL
	const double  SCALE_2 = 1000.0;		//转换成nL
	const int16_t SCALE_3 = 256;			//电机细分步数
	
	double raw_liq = Volume*SCALE_1*SCALE_2*SCALE_3/g_MCBConfig.PumpOneStep[Channel-1];
	
	int32_t temp_position = llround(raw_liq);
	
	if(g_MCBConfig.ALLPumpDir == Dir)
		TMC5240_MoveTo(Channel,temp_position);
	else
		TMC5240_MoveTo(Channel,-temp_position);
	
	switch (Channel)
	{
		case 1:
			Channel1_MotorStatus.Busy = USER_BUSY;
			PumpTube_StartMotor(Channel);
			if (Waits == Wait)
				while (Channel1_MotorStatus.Busy == USER_BUSY);		
			break;

		case 2:
			Channel2_MotorStatus.Busy = USER_BUSY;
			PumpTube_StartMotor(Channel);
			if (Waits == Wait)
				while (Channel2_MotorStatus.Busy == USER_BUSY);		
			break;

		case 3:
			Channel3_MotorStatus.Busy = USER_BUSY;
			PumpTube_StartMotor(Channel);
			if (Waits == Wait)
				while (Channel3_MotorStatus.Busy == USER_BUSY);		
			break;

		case 4:
			Channel4_MotorStatus.Busy = USER_BUSY;
			PumpTube_StartMotor(Channel);
			if (Waits == Wait)
				while (Channel4_MotorStatus.Busy == USER_BUSY);		
			break;
		
		default:
			break;
	}
}
	
/**
  * @brief  蠕动泵持续动作动作
  * @param  Dir:    正传或反转，加注或回吸
  * @param  Waits:  是否需要等待，死等
  * @retval None
  */
void PumpRotate(uint8_t Channel,MotorDIR Dir,MotorWait Waits)
{
	if ((Channel < 1U) || (Channel > 4U))
		return;
	if(g_MCBConfig.ALLPumpDir == Dir)
		TMC5240_Rotate(Channel,g_MCBConfig.PumpSpeed[Channel-1]);
	else
		TMC5240_Rotate(Channel,-g_MCBConfig.PumpSpeed[Channel-1]);
	
	switch (Channel)
	{
	case 1:
		Channel1_MotorStatus.Busy = USER_BUSY;
		PumpTube_StartMotor(Channel);
		if (Waits == Wait)
			while (Channel1_MotorStatus.Busy == USER_BUSY);		
		break;

	case 2:
		Channel2_MotorStatus.Busy = USER_BUSY;
		PumpTube_StartMotor(Channel);
		if (Waits == Wait)
			while (Channel2_MotorStatus.Busy == USER_BUSY);		
		break;

	case 3:
		Channel3_MotorStatus.Busy = USER_BUSY;
		PumpTube_StartMotor(Channel);
		if (Waits == Wait)
			while (Channel3_MotorStatus.Busy == USER_BUSY);		
		break;

	case 4:
		Channel4_MotorStatus.Busy = USER_BUSY;
		PumpTube_StartMotor(Channel);
		if (Waits == Wait)
			while (Channel4_MotorStatus.Busy == USER_BUSY);		
		break;
	
	default:
		break;
	}
}
	
/**
  * @brief  蠕动泵停止
  * @param  Dir:    正传或反转，加注或回吸
  * @param  Volume: 加液体积，以单步加液量为准,单位mL
  * @param  Waits:  是否需要等待，死等
  * @retval None
  */
void PumpStop(uint8_t Channel)
{
	if ((Channel < 1U) || (Channel > 4U))
		return;
	TMC5240_Stop(Channel);
	PumpTube_StopMotor(Channel, (int32_t)TMC5240_ReadRegister(Channel, TMC5240_XACTUAL));
}
	
	
	
	
	
	
	
	
	
	
	
	
	

