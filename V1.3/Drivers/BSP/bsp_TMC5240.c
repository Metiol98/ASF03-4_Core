/**************************************************************************************************
 * @file         bsp_TMC5240.c
 * @brief        SPI 总线电机驱动文件
 * @note         通用驱动层，与硬件平台无关
 *               硬件相关配置请查看 bsp_TMC5240.h
 *
 * @attention
 *
 * 1. 本文件为通用驱动，不包含硬件引脚定义，移植时只需修改 bsp_motor.h
 * 2. 支持不同 SPI 端口、不同 EN / CS 引脚
 * 3. 驱动仅实现寄存器操作、使能控制、运行逻辑，不依赖具体硬件
 * 4. 更换硬件平台时，此文件无需修改
 *
 * @author       Q1uuuu
 * @date         2026-04-15
 * @version      V1.0
 *************************************************************************************************/

#include "bsp_TMC5240.h"

#define SPITIMESOUT		500			//SPI超时时间

MotorStatusSTR	Channel1_MotorStatus;     /* 电机1状态结构体 */
MotorStatusSTR	Channel2_MotorStatus;     /* 电机1状态结构体 */
MotorStatusSTR	Channel3_MotorStatus;     /* 电机1状态结构体 */
MotorStatusSTR	Channel4_MotorStatus;     /* 电机1状态结构体 */

//电机IHOUD,IRUN电流默认值
uint8_t Channel_IRUN = 20;
uint8_t Channel_IHOLD = 5;

//延时函数 (Freertos使用)
static void TMC5240_Delay_us(uint32_t Delay_us)
{
	uint32_t Delay_Times = Delay_us*10;
	do{
		Delay_Times--;
		//asm("nop");
	}while(Delay_Times);
}

/**
  * @brief  TMC5240初始化
  * @param  None
  * @retval None
  */
void TMC5240_init(void)
{
	TMC5240_SPI_CS1_OFF;
	TMC5240_SPI_CS2_OFF;
	TMC5240_SPI_CS3_OFF;
	TMC5240_SPI_CS4_OFF;

	TMC5240_SPI_EN1_OFF;
	TMC5240_SPI_EN2_OFF;
	TMC5240_SPI_EN3_OFF;
	TMC5240_SPI_EN4_OFF;

	TMC5240_SetFirstState(1);
	TMC5240_SetFirstState(2);
	TMC5240_SetFirstState(3);
	TMC5240_SetFirstState(4);

	TMC5240_SPI_EN1_ON;
	TMC5240_SPI_EN2_ON;
	TMC5240_SPI_EN3_ON;
	TMC5240_SPI_EN4_ON;
}

/* SPI发送函数 */
static uint8_t Device_SPI_Transmit(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
    uint8_t status = 0;
    
    status = HAL_SPI_TransmitReceive(&TMC5240_SPI, pTxData, pRxData, Size, SPITIMESOUT);
    
    return status;
}

/**
  * @brief  向TMC5160寄存器发送数据
	* @param  Channel	：通道1~4
  * @param  Regaddr	：寄存器地址
	* @param  Data		：写入的数据
	* @param  RW_Flag	：读写标志
			@arg	TMC_READ		读取状态
			@arg	TMC_WRITE		写入数据
  * @retval None
  */
uint32_t TMC5240_WriteReadRegister(uint8_t Channel, uint8_t Regaddr, uint32_t Data, uint8_t RW_Flag)
{    
  uint8_t Temp[8];
  uint8_t i=0;
  uint8_t Txdata[4];
	uint32_t Rxdata=0;
	
	memset(Temp,0,8);

	switch (Channel)
	{
		case 1:
			TMC5240_SPI_CS1_ON;
			break;
		
		case 2:
			TMC5240_SPI_CS2_ON;
			break;

		case 3:
			TMC5240_SPI_CS3_ON;
			break;

		case 4:
			TMC5240_SPI_CS4_ON;
			break;
		
		default:
			break;
	}
	
	Txdata[0] = (Data>>24) & 0xFF;
	Txdata[1] = (Data>>16) & 0xFF;
	Txdata[2] = (Data>> 8) & 0xFF;
	Txdata[3] =  Data			 & 0xFF;
	
	CRDDATA_Typdef CRData;
	memset(&CRData,0,sizeof(CRDDATA_Typdef));
	
	CRData.WEN = RW_Flag;
	CRData.RS = Regaddr;
	memcpy(&Temp,&CRData,sizeof(CRDDATA_Typdef));
	
	i = 1;
	while(i <= 4)
	{
		Temp[i] = Txdata[i-1];
		i++;
	}
	
	Device_SPI_Transmit(Temp,Temp,i);
	
	switch (Channel)
	{
		case 1:
			TMC5240_SPI_CS1_OFF;
			break;
		
		case 2:
			TMC5240_SPI_CS2_OFF;
			break;

		case 3:
			TMC5240_SPI_CS3_OFF;
			break;

		case 4:
			TMC5240_SPI_CS4_OFF;
			break;
		
		default:
			break;
	}
	
	/* 数据处理 */
	Rxdata = 	Rxdata 		 | Temp[1];
	Rxdata = (Rxdata<<8) | Temp[2];
	Rxdata = (Rxdata<<8) | Temp[3];
	Rxdata = (Rxdata<<8) | Temp[4];

	return Rxdata;
}

/**
  * @brief  向TMC5160寄存器读取数据
	* @param  Channel	：通道1~4
  * @param  Regaddr	：寄存器地址
  * @retval 返回读出的数据
  */
uint32_t TMC5240_ReadRegister(uint32_t Channel, uint8_t Regaddr)
{
	uint32_t Datagram=0;
	
	TMC5240_WriteReadRegister(Channel,Regaddr,0,TMC_READ);
	Datagram = TMC5240_WriteReadRegister(Channel,Regaddr,0,TMC_READ);
	
	return Datagram;
}

/**
  * @brief  给TMC5240芯片寄存器提供初始值
	* @param  Channel	：通道1~4
  * @retval None.
  */
static void TMC5240_SetFirstState(uint8_t Channel)
{
	uint8_t data[8];
	memset(data,0x00,4);
	
	/* 步进和方向配置 */
	TMC5240_WriteReadRegister(Channel,TMC5240_CHOPCONF,		0x000100C3  ,TMC_WRITE);		//PAGE46:	TOFF=3 ; HSTRT=3 ; HEND=1 ; TBL=2 ; CHM=1
	TMC5240_WriteReadRegister(Channel,TMC5240_DRV_CONF,		0x00000002  ,TMC_WRITE);		//PAGE46:	TOFF=3 ; HSTRT=3 ; HEND=1 ; TBL=2 ; CHM=1
	TMC5240_WriteReadRegister(Channel,TMC5240_IHOLD_IRUN, 0x00061405  ,TMC_WRITE);		//PAGE33: IHOD_IRUN: IHOLD=10 ; IRUN=31 ; IHOLDDELAY=6
	TMC5240_WriteReadRegister(Channel,TMC5240_TPOWERDOWN, 0x0000000A  ,TMC_WRITE);		//PAGE33:	TPOWERDOWN: 电机静止到电流减小之间的延时
	TMC5240_WriteReadRegister(Channel,TMC5240_GCONF,			0x00000004  ,TMC_WRITE);		//PAGE27:	GCONF:	en_pwm_mode = 1
	TMC5240_WriteReadRegister(Channel,TMC5240_SG4_THRS,		0x00000200	,TMC_WRITE);/* 平滑模式使能，stealthChop和spreadCycle切换时更平滑； */
	TMC5240_WriteReadRegister(Channel,TMC5240_PWMCONF,		0x00043B45  ,TMC_WRITE);		/* 申辰42开环电机0x00043B45
																																						* 申辰42闭环电机0x00041439
																																						* 速度前馈控制模式，不需要自整定 */
	TMC5240_WriteReadRegister(Channel,TMC5240_SHORT_CONF, 0x00030D0D  ,TMC_WRITE);
	TMC5240_WriteReadRegister(Channel,TMC5240_TPWMTHRS,		67  				,TMC_WRITE);		//PAGE33:	TPWM_THRS=500,对应切换速度35000=ca.30RPM
	
	/* 斜坡发生器运动配置 */
	TMC5240_WriteReadRegister(Channel,TMC5240_A1,					3000			,TMC_WRITE);		//PAGE35:	A1=1000，第一阶段加速度
	TMC5240_WriteReadRegister(Channel,TMC5240_V1,					30*1193		,TMC_WRITE);		//PAGE35: V1=50000加速度阈值速度V1
	TMC5240_WriteReadRegister(Channel,TMC5240_A2,					6000			,TMC_WRITE);		//PAGE35:	A1=1000，第一阶段加速度
	TMC5240_WriteReadRegister(Channel,TMC5240_V2,					180*1193		,TMC_WRITE);
	TMC5240_WriteReadRegister(Channel,TMC5240_AMAX,				3000			,TMC_WRITE);		//PAGE35: AMAX=500大于V1加速度
	TMC5240_WriteReadRegister(Channel,TMC5240_VMAX,				200*1193	,TMC_WRITE);		//PAGE35: VMAX=238609
	TMC5240_WriteReadRegister(Channel,TMC5240_DMAX,				12000			,TMC_WRITE);		//PAGE35: DMAX=700大于V1减速度
	TMC5240_WriteReadRegister(Channel,TMC5240_D2,					16000			,TMC_WRITE);		//PAGE35: D1=1400小于V1减速度
	TMC5240_WriteReadRegister(Channel,TMC5240_D1,					10000			,TMC_WRITE);		//PAGE35: D1=1400小于V1减速度
	TMC5240_WriteReadRegister(Channel,TMC5240_VSTOP,			10				,TMC_WRITE);		//PAGE35: VSTOP=10停止速度，至少设置为0
	TMC5240_WriteReadRegister(Channel,TMC5240_RAMPMODE,		0					,TMC_WRITE);		//PAGE35: RAMPMODE=0位置模式
	
	TMC5240_ClearPosition(Channel);
}

/**
  * @brief  清除实际位置和目标位置
	* @param  Channel	：通道1~4
  * @retval None.
  */
void TMC5240_ClearPosition(uint8_t Channel)
{
	TMC5240_WriteReadRegister(Channel,TMC5240_SWMODE,0x0803,TMC_WRITE);		//停止标志位
	TMC5240_Delay_us(50);
	TMC5240_WriteReadRegister(Channel,TMC5240_XACTUAL,0,TMC_WRITE);				//清除实际位置
	TMC5240_WriteReadRegister(Channel,TMC5240_XTARGET,0,TMC_WRITE);				//清除目标位置
	TMC5240_WriteReadRegister(Channel,TMC5240_RAMPMODE,3,TMC_WRITE);			
	TMC5240_WriteReadRegister(Channel,TMC5240_SWMODE,0,TMC_WRITE);				//清除停止标志位
}

/**
  * @brief  以一个速度旋转
	* @param  Channel	：通道1~4
	* @param  Speed		：旋转速度，可以是正负的，不同方向
  * @retval None.
  */
void TMC5240_Rotate(uint8_t Channel, int32_t Speed)
{
	TMC5240_ClearPosition(Channel);
	TMC5240_WriteReadRegister(Channel,TMC5240_SWMODE,12,TMC_WRITE);
	TMC5240_WriteReadRegister(Channel,TMC5240_VMAX,abs(Speed)*1193,TMC_WRITE);
	TMC5240_WriteReadRegister(Channel,TMC5240_RAMPMODE,(Speed >= 0) ? 1 : 2,TMC_WRITE);
}

/**
  * @brief  运动到固定的位置
  * @param  Channel	：通道1~4
	* @param  position：位置
  * @retval None.
  */
void TMC5240_MoveTo(uint8_t Channel, int32_t position)
{
	TMC5240_ClearPosition(Channel);
	TMC5240_WriteReadRegister(Channel,TMC5240_SWMODE,12,TMC_WRITE);
	TMC5240_WriteReadRegister(Channel,TMC5240_RAMPMODE,0,TMC_WRITE);
	TMC5240_WriteReadRegister(Channel,TMC5240_XTARGET,position,TMC_WRITE);
}

/**
  * @brief  停止，按照设定的减速度停止，速度不会马上到0
	* @param  Channel	：通道1~4
  * @retval None.
  */
void TMC5240_Stop(uint8_t Channel)
{
	TMC5240_WriteReadRegister(Channel,TMC5240_SWMODE,0x080F,TMC_WRITE);	//软停
	for(int i=0;i<30;i++);
	return;
}

/**
  * @brief  立即停止，速度马上减为0
	* @param  Channel	：通道1~4
  * @retval None.
  */
void TMC5240_StopNow(uint8_t Channel)
{
	TMC5240_WriteReadRegister(Channel,TMC5240_SWMODE,15,TMC_WRITE);
	TMC5240_Delay_us(3000);
}

/**
  * @brief  设置旋转速度
	* @param  Channel	：通道1~4
	* @param  Speed		：旋转速度
  * @retval None.
  */
void TMC5240_SetSpeed(uint8_t Channel, float Speed)
{
	TMC5240_WriteReadRegister(Channel,TMC5240_VMAX,Speed*1197,TMC_WRITE);
}

/**
  * @brief  设置运行电流
	* @param  Channel	：通道1~4
	* @param  IRUN		：运行电流
  * @retval None.
  */
void TMC5240_SetIRUN(uint8_t Channel, uint8_t IRUN)
{
	uint32_t temp=0;
	
	temp = TMC5240_ReadRegister(Channel,TMC5240_IHOLD_IRUN);
	temp = (temp &0xFFFF00FF) | (IRUN<<8);
	TMC5240_WriteReadRegister(Channel,TMC5240_IHOLD_IRUN,temp,TMC_WRITE);
}

/**
  * @brief  设置保持电流
	* @param  Channel	：通道1~4
	* @param  IHOLD		：保持电流
  * @retval None.
  */
void TMC5240_SetIHOLD(uint8_t Channel, uint8_t IHOLD)
{
	uint32_t temp=0;
	
	temp = TMC5240_ReadRegister(Channel,TMC5240_IHOLD_IRUN);
	temp = (temp &0xFFFFFF00) | IHOLD;
	TMC5240_WriteReadRegister(Channel,TMC5240_IHOLD_IRUN,temp,TMC_WRITE);
}

/**
  * @brief  设置运动相关的参数，主要是加减速等
	* @param  Channel	：通道1~4
	* @param  Regaddr		：地址
	* @param  Data			：数据
  * @retval None.
  */
void TMC5240_SetMotionAP(uint8_t Channel, int8_t Regaddr,int32_t Data)
{
	switch(Regaddr)
	{
		case TMC5240_VSTART://起始速度
			TMC5240_WriteReadRegister(Channel,TMC5240_VSTART,	Data,TMC_WRITE);
			break;
		
		case TMC5240_A1://第一段加速度
			TMC5240_WriteReadRegister(Channel,TMC5240_A1,	Data,TMC_WRITE);
			break;
		
		case TMC5240_V1://中间速度
			TMC5240_WriteReadRegister(Channel,TMC5240_V1,	Data,TMC_WRITE);
			break;
		
		case TMC5240_AMAX://第二段加速度
			TMC5240_WriteReadRegister(Channel,TMC5240_AMAX,	Data,TMC_WRITE);
			break;
		
		case TMC5240_VMAX://最大速度
			TMC5240_WriteReadRegister(Channel,TMC5240_VMAX,	Data,TMC_WRITE);
			break;
		
		case TMC5240_DMAX://第一段减速度
			TMC5240_WriteReadRegister(Channel,TMC5240_DMAX,	Data,TMC_WRITE);
			break;
		
		case TMC5240_D1://第二段减速度
			TMC5240_WriteReadRegister(Channel,TMC5240_AMAX,	Data,TMC_WRITE);
			break;
		
		case TMC5240_VSTOP://停止速度，到达该速度直接停止
			TMC5240_WriteReadRegister(Channel,TMC5240_VMAX,	Data,TMC_WRITE);
			break;
		
		case TMC5240_TZEROWAIT://静止后的等待时间，防止速度突变
			TMC5240_WriteReadRegister(Channel,TMC5240_TZEROWAIT,	Data,TMC_WRITE);
			break;
		
		default:
			break;
	}
}

/**
  * @brief  获取电机状态
	* @param  Channel	：通道1~4
  * @retval None.
  */
void TMC5240_QueryMotorStatus(uint8_t Channel)
{
	switch (Channel)
	{
		case 1:
			//Channel1_MotorStatus.CHOPCONF   = TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF);
			//Channel1_MotorStatus.DRV_STATUS = TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS);
			//Channel1_MotorStatus.GSTAT			= TMC5240_ReadRegister(Channel,TMC5240_GSTAT);
			Channel1_MotorStatus.RAMP_STAT  = TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT);

			if(	((Channel1_MotorStatus.RAMP_STAT & 0x00000200) != 0)	/* 到达目标位置 */
				||(((Channel1_MotorStatus.RAMP_STAT	& 0x00000030) != 0) && ((Channel1_MotorStatus.RAMP_STAT	& 0x00000400) != 0))	/* 软停启用且停稳 */
			)
			Channel1_MotorStatus.Busy = 0;	/* 电机不忙 */
			break;

		case 2:
			//Channel2_MotorStatus.CHOPCONF   = TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF);
			//Channel2_MotorStatus.DRV_STATUS = TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS);
			//Channel2_MotorStatus.GSTAT			= TMC5240_ReadRegister(Channel,TMC5240_GSTAT);
			Channel2_MotorStatus.RAMP_STAT  = TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT);

			if(	((Channel2_MotorStatus.RAMP_STAT & 0x00000200) != 0)	/* 到达目标位置 */
				||(((Channel2_MotorStatus.RAMP_STAT	& 0x00000030) != 0) && ((Channel2_MotorStatus.RAMP_STAT	& 0x00000400) != 0))	/* 软停启用且停稳 */
			)
			Channel2_MotorStatus.Busy = 0;	/* 电机不忙 */
			break;

		case 3:
			//Channel3_MotorStatus.CHOPCONF   = TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF);
			//Channel3_MotorStatus.DRV_STATUS = TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS);
			//Channel3_MotorStatus.GSTAT			= TMC5240_ReadRegister(Channel,TMC5240_GSTAT);
			Channel3_MotorStatus.RAMP_STAT  = TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT);

			if(	((Channel3_MotorStatus.RAMP_STAT & 0x00000200) != 0)	/* 到达目标位置 */
				||(((Channel3_MotorStatus.RAMP_STAT	& 0x00000030) != 0) && ((Channel3_MotorStatus.RAMP_STAT	& 0x00000400) != 0))	/* 软停启用且停稳 */
			)
			Channel3_MotorStatus.Busy = 0;	/* 电机不忙 */
			break;
		
		case 4:
			//Channel4_MotorStatus.CHOPCONF   = TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF);
			//Channel4_MotorStatus.DRV_STATUS = TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS);
			//Channel4_MotorStatus.GSTAT			= TMC5240_ReadRegister(Channel,TMC5240_GSTAT);
			Channel4_MotorStatus.RAMP_STAT  = TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT);

			if(	((Channel4_MotorStatus.RAMP_STAT & 0x00000200) != 0)	/* 到达目标位置 */
				||(((Channel4_MotorStatus.RAMP_STAT	& 0x00000030) != 0) && ((Channel4_MotorStatus.RAMP_STAT	& 0x00000400) != 0))	/* 软停启用且停稳 */
			)
			Channel4_MotorStatus.Busy = 0;	/* 电机不忙 */
			break;
	
		default:
			break;
	}

}







