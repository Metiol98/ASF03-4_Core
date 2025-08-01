#include "HEAD.h"

TMC5240MotorStatus Channel1_MotorStatus;
TMC5240MotorStatus Channel2_MotorStatus;
TMC5240MotorStatus Channel3_MotorStatus;
TMC5240MotorStatus Channel4_MotorStatus;

/*********************************************************
函数名称：WriteCode(uint8_t Channel,uint8_t Add,int32_t Data,uint8_t RW)
描    述：向TMC5240寄存器发送数据
形    参：Channel	：板载通道，1-4
					Add			：寄存器地址
					Data		：写入的数据
*********************************************************/
void TMC5240_WriteRegister(uint8_t Channel,int8_t Add,int32_t Data)
{
	uint8_t datatemp[4];
	
	switch(Channel)
	{
		case 1:
			Motor_SPI_CS1_ON;
			break;
		case 2:
			Motor_SPI_CS2_ON;
			break;
		case 3:
			Motor_SPI_CS3_ON;
			break;
		case 4:
			Motor_SPI_CS4_ON;
			break;
		default:
			return;
	}
	Delay(10);//延时，等待拉低电平使能CSx
	
	datatemp[3]=Data>>24;
	datatemp[2]=Data>>16;
	datatemp[1]=Data>>8;
	datatemp[0]=Data;
	
	Motor_SPI_ReadWriteByte(Add|0x80);
	for(int i=4;i>0;i--)
	{
		Motor_SPI_ReadWriteByte(datatemp[i-1]);
	}
	Delay(30);//必要延时
	//CS电平拉高，锁存寄存器，发送的数据才有效
	switch(Channel)
	{
		case 1:
			Motor_SPI_CS1_OFF;
			break;
		case 2:
			Motor_SPI_CS2_OFF;
			break;
		case 3:
			Motor_SPI_CS3_OFF;
			break;
		case 4:
			Motor_SPI_CS4_OFF;
			break;
		default:
			return;
	}
	return;
}

/*********************************************************
函数名称：TMC5240_ReadCode(uint8_t Channel,uint8_t Add)
描    述：向TMC5240寄存器读取数据
形    参：Channel	：板载通道，1-4
					Add			：寄存器地址
					SorD		：状态或是数据
*********************************************************/
uint32_t TMC5240_ReadRegister(uint8_t Channel,uint8_t Add,uint8_t SorD)
{
	uint32_t Data=0;
	uint8_t datatemp[5];
	switch(Channel)
	{
		case 1:
			Motor_SPI_CS1_ON;
			break;
		case 2:
			Motor_SPI_CS2_ON;
			break;
		case 3:
			Motor_SPI_CS3_ON;
			break;
		case 4:
			Motor_SPI_CS4_ON;
			break;
		default:
			return 0xFFFFFFFF;
	}
	Delay(10);//延时，等待拉低电平使能CSx

	Motor_SPI_ReadWriteByte(Add);
	for(int i=4;i>0;i--)
		Motor_SPI_ReadWriteByte(DUMMY);

	Delay(30);//必要延时
	//CS电平拉高，锁存寄存器，发送的数据才有效
	switch(Channel)
	{
		case 1:
			Motor_SPI_CS1_OFF;
			break;
		case 2:
			Motor_SPI_CS2_OFF;
			break;
		case 3:
			Motor_SPI_CS3_OFF;
			break;
		case 4:
			Motor_SPI_CS4_OFF;
			break;
		default:
			return 0xFFFFFFFF;
	}
	
	switch(Channel)
	{
		case 1:
			Motor_SPI_CS1_ON;
			break;
		case 2:
			Motor_SPI_CS2_ON;
			break;
		case 3:
			Motor_SPI_CS3_ON;
			break;
		case 4:
			Motor_SPI_CS4_ON;
			break;
		default:
			return 0xFFFFFFFF;
	}
	Delay(10);//延时，等待拉低电平使能CSx
	
	datatemp[0]=Motor_SPI_ReadWriteByte(Add);
	for(int i=1;i<5;i++)
		datatemp[i]=Motor_SPI_ReadWriteByte(DUMMY);
	
	Delay(30);//必要延时
	//CS电平拉高，锁存寄存器，发送的数据才有效
	switch(Channel)
	{
		case 1:
			Motor_SPI_CS1_OFF;
			break;
		case 2:
			Motor_SPI_CS2_OFF;
			break;
		case 3:
			Motor_SPI_CS3_OFF;
			break;
		case 4:
			Motor_SPI_CS4_OFF;
			break;
		default:
			return 0xFFFFFFFF;
	}
	//【数据处理】
	Data=Data|datatemp[1];
	Data=(Data<<8)|datatemp[2];
	Data=(Data<<8)|datatemp[3];
	Data=(Data<<8)|datatemp[4];
	
	if(SorD==0)//获取状态
		return datatemp[0];
	else//获取数据
		return Data;
}
/****************************************************/
/****************************************************/
//【TMC5240初始化】
/***********************************
*开机电机驱动器参数初始化，都是默认值。
*
*
*
*
***********************************/
void TMC5240_Init(uint8_t Channel)
{
	if((Channel<1)||(Channel>4))
		return;
	
	//步进和方向模式
	TMC5240_WriteRegister(Channel,TMC5240_CHOPCONF,		0x000100C3);//0x00010383自整定  0x000100C3 斩波器配置256step
	TMC5240_WriteRegister(Channel,TMC5240_DRV_CONF,		0x00000002);//**电流范围3A，电流电阻为16K，I=36/16=2.25A；
	TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
	TMC5240_WriteRegister(Channel,TMC5240_TPOWERDOWN,	0x0000000A);
	TMC5240_WriteRegister(Channel,TMC5240_GCONF,			0x00000004);//stealthChop模式打开
	TMC5240_WriteRegister(Channel,TMC5240_SG4_THRS,		0x00000200);//平滑模式使能，stealthChop和spreadCycle切换时更平滑；
	TMC5240_WriteRegister(Channel,TMC5240_PWMCONF,		0x00043B45);//速度前馈控制模式，不需要自整定
	//TMC5240_WriteRegister(Channel,TMC5240_SHORT_CONF,	0x00030D0D);//短路保护，
	TMC5240_WriteRegister(Channel,TMC5240_TPWMTHRS,		67);				//67 210RPM一下切换模式stealthChop，以上是spreadCycle
	
	
	//速度相关驱动特性寄存器
	TMC5240_WriteRegister(Channel,TMC5240_A1,				3000);				//0-240rpm的加速时间为1s
	TMC5240_WriteRegister(Channel,TMC5240_V1,				30*1193);			//180rpm
	TMC5240_WriteRegister(Channel,TMC5240_A2,				6000);				//180rpm
	TMC5240_WriteRegister(Channel,TMC5240_V2,				180*1193);		//180rpm
	TMC5240_WriteRegister(Channel,TMC5240_AMAX,			3000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_VMAX,			200*1193);		//默认240rpm
	TMC5240_WriteRegister(Channel,TMC5240_DMAX,			12000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_D2,				16000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_D1,				10000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_VSTOP,		10);					//停止速度，默认10
	TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE,	0);						//速度模式
	
	TMC5240_ClearPosition(Channel);
	return;
}

/****************************************************/
/****************************************************/
void TMC5240_FlagInit(void)
{
//	Channel1_Vmax=300;
//	Channel2_Vmax=300;
//	Channel3_Vmax=300;
//	Channel4_Vmax=300;
//	
//	Channel1_IRUN=25;
//	Channel2_IRUN=25;
//	Channel3_IRUN=25;
//	Channel4_IRUN=25;

//	Channel1_IHOLD=10;
//	Channel2_IHOLD=10;
//	Channel3_IHOLD=10;
//	Channel4_IHOLD=10;
}




//【TMC5240基础功能函数】
/*********************************************************
函数名称：TMC5240_Rotate(uint8_t Channel,int32_t velocity)
描    述：以一个速度旋转
形    参：Channel	：板载通道，1-4
					velocity：旋转速度，可以是正负的，不同方向
*********************************************************/
void TMC5240_Rotate(uint8_t Channel,int32_t velocity)
{
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,12);//清除停止标志位
	TMC5240_WriteRegister(Channel,TMC5240_VMAX, abs(velocity)*298);
	TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE, (velocity >= 0)? 1 : 2);
  return;
}


/*********************************************************
函数名称：TMC5240_MoveTo(uint8_t Channel,int32_t velocity)
描    述：运动到固定的位置
形    参：Channel	：板载通道，1-4
					position：位置
*********************************************************/
void TMC5240_MoveTo(uint8_t Channel,int32_t position)
{
	TMC5240_WriteRegister(Channel,TMC5240_DMAX,			12000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_D2,				16000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_D1,				10000);				//0-240rpm的加速时间为0.75s
	
	TMC5240_ClearPosition(Channel);
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,12);//清除停止标志位
	TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE,0);
	TMC5240_WriteRegister(Channel,TMC5240_XTARGET, position);
  return;
}

/*********************************************************
函数名称：TMC5240_MoveTo(uint8_t Channel,int32_t velocity)
描    述：持续运动
形    参：Channel	：板载通道，1-4
					position：位置
*********************************************************/
void TMC5240_MoveGo(uint8_t Channel,uint8_t Dir)
{
	TMC5240_ClearPosition(Channel);
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,12);//清除停止标志位
	if(Dir==1)
		TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE,1);
	else
		TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE,2);
  return;
}

/*********************************************************
函数名称：TMC5240_ClearPosition(uint8_t Channel)
描    述：清除实际位置和目标位置
形    参：Channel	：板载通道，1-4
					
*********************************************************/
void TMC5240_ClearPosition(uint8_t Channel)
{
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,15);//清除停止标志位
	Delay(30);
	TMC5240_WriteRegister(Channel,TMC5240_XACTUAL,0);	//清除实际位置
	TMC5240_WriteRegister(Channel,TMC5240_XTARGET,0);	//清除目标位置
	TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE,3); 					
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,12);
	return;
}

/*********************************************************
函数名称：TMC5240_Stop(uint8_t Channel)
描    述：停止，按照设定的减速度停止，速度不会马上到0
形    参：Channel	：板载通道，1-4

*********************************************************/
void TMC5240_Stop(uint8_t Channel)
{
	TMC5240_WriteRegister(Channel,TMC5240_DMAX,			18000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_D2,				20000);				//0-240rpm的加速时间为0.75s
	TMC5240_WriteRegister(Channel,TMC5240_D1,				16000);				//0-240rpm的加速时间为0.75s
	
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,   0x080F);
	Delay(30);
	return;
}
/*********************************************************
函数名称：TMC5240_StopNow(uint8_t Channel)
描    述：立即停止，速度马上减为0
形    参：Channel	：板载通道，1-4
					
*********************************************************/
void TMC5240_StopNow(uint8_t Channel)
{
	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,15);
	Delay(30000);
	
	switch(Channel)
	{
		case 1://
			Motor_SPI_EN1_OFF;
			Delay(30000);
			Motor_SPI_EN1_ON;
			break;
		case 2://
			Motor_SPI_EN2_OFF;
			Delay(30000);
			Motor_SPI_EN2_ON;
			break;
		case 3://
			Motor_SPI_EN3_OFF;
			Delay(30000);
			Motor_SPI_EN3_ON;
			break;
		case 4://
			Motor_SPI_EN4_OFF;
			Delay(30000);
			Motor_SPI_EN4_ON;
			break;
		default:
			break;
	}
	
//	TMC5240_WriteRegister(Channel,TMC5240_RAMPMODE,3); 					
//	TMC5240_WriteRegister(Channel,TMC5240_SWMODE,12);
	return;
}




/*********************************************************
函数名称：TMC5240_SetIRUN(uint8_t Channel,uint8_t IRUN)
描    述：运行电流设置
形    参：IRUN	：运行电流，0-31
					IHOLD	：保持电流，0-31
*********************************************************/
void TMC5240_SetIRUN(uint8_t Channel,uint8_t IRUN)
{
//	uint32_t IrunTemp=0;
//	
//	switch(Channel)
//	{
//		case 1://
//			Channel1_IRUN=IRUN;
//			IrunTemp=0x06;
//			IrunTemp=(IrunTemp<<8)|Channel1_IRUN;
//			IrunTemp=(IrunTemp<<8)|Channel1_IHOLD;
//	
//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IrunTemp);
//			break;
//		case 2://起始速度
//			Channel2_IRUN=IRUN;
//			IrunTemp=0x06;
//			IrunTemp=(IrunTemp<<8)|Channel2_IRUN;
//			IrunTemp=(IrunTemp<<8)|Channel2_IHOLD;
//	
//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IrunTemp);
//			break;
//		case 3://起始速度
//			Channel3_IRUN=IRUN;
//			IrunTemp=0x06;
//			IrunTemp=(IrunTemp<<8)|Channel3_IRUN;
//			IrunTemp=(IrunTemp<<8)|Channel3_IHOLD;
//	
//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IrunTemp);
//			break;
//		case 4://起始速度
//			Channel4_IRUN=IRUN;
//			IrunTemp=0x06;
//			IrunTemp=(IrunTemp<<8)|Channel4_IRUN;
//			IrunTemp=(IrunTemp<<8)|Channel4_IHOLD;
//	
//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IrunTemp);
//			break;
//		default:
//			break;
//	}
	return;
}
/*********************************************************
函数名称：TMC5240_SetIRUN(uint8_t Channel,uint8_t IRUN)
描    述：保持电流设置
形    参：IRUN	：运行电流，0-31
					IHOLD	：保持电流，0-31
*********************************************************/
void TMC5240_SetIHOLD(uint8_t Channel,uint8_t IHOLD)
{
//	uint32_t IholdTemp=0;
	
//	switch(Channel)
//	{
//		case 1://
//			Channel1_IHOLD=IHOLD;
//			IholdTemp=0x06;
//			IholdTemp=(IholdTemp<<8)|Channel1_IRUN;
//			IholdTemp=(IholdTemp<<8)|Channel1_IHOLD;

//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IholdTemp);
//			break;
//		case 2://起始速度
//			Channel2_IHOLD=IHOLD;
//			IholdTemp=0x06;
//			IholdTemp=(IholdTemp<<8)|Channel2_IRUN;
//			IholdTemp=(IholdTemp<<8)|Channel2_IHOLD;

//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IholdTemp);
//			break;
//		case 3://起始速度
//			Channel3_IHOLD=IHOLD;
//			IholdTemp=0x06;
//			IholdTemp=(IholdTemp<<8)|Channel3_IRUN;
//			IholdTemp=(IholdTemp<<8)|Channel3_IHOLD;

//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IholdTemp);
//			break;
//		case 4://起始速度
//			Channel4_IHOLD=IHOLD;
//			IholdTemp=0x06;
//			IholdTemp=(IholdTemp<<8)|Channel4_IRUN;
//			IholdTemp=(IholdTemp<<8)|Channel4_IHOLD;

//			TMC5240_WriteRegister(Channel,TMC5240_IHOLD_IRUN,	IholdTemp);
//			break;
//		default:
//			break;
//	}
	return;
}

/*********************************************************
函数名称：TMC5240_SetMotionAP(uint8_t Channel,uint8_t readWrite,int8_t Add,int32_t Data)
描    述：设置运动相关的参数，主要是加减速等
形    参：Channel		：板载通道，1-4
					Add				：地址
					Data			：数据
*********************************************************/
void TMC5240_SetMotionAP(uint8_t Channel,int8_t Add,int32_t Data)
{
	switch(Add)
	{
		case TMC5240_VSTART://起始速度
			TMC5240_WriteRegister(Channel,TMC5240_VSTART,	Data);
			break;
		
		case TMC5240_A1://第一段加速度
			TMC5240_WriteRegister(Channel,TMC5240_A1,	Data);
			break;
		
		case TMC5240_V1://中间速度
			TMC5240_WriteRegister(Channel,TMC5240_V1,	Data);
			break;
		
		case TMC5240_AMAX://第二段加速度
			TMC5240_WriteRegister(Channel,TMC5240_AMAX,	Data);
			break;
		
		case TMC5240_VMAX://最大速度
			TMC5240_WriteRegister(Channel,TMC5240_VMAX,	Data);
			break;
		
		case TMC5240_DMAX://第一段减速度
			TMC5240_WriteRegister(Channel,TMC5240_DMAX,	Data);
			break;
		
		case TMC5240_D1://第二段减速度
			TMC5240_WriteRegister(Channel,TMC5240_AMAX,	Data);
			break;
		
		case TMC5240_VSTOP://停止速度，到达该速度直接停止
			TMC5240_WriteRegister(Channel,TMC5240_VMAX,	Data);
			break;
		
		case TMC5240_TZEROWAIT://静止后的等待时间，防止速度突变
			TMC5240_WriteRegister(Channel,TMC5240_TZEROWAIT,	Data);
			break;
		
		default:
			break;
	}
}

/*********************************************************
函数名称：TMC5240_QueryMotorStatus(uint8_t Channel)
描    述：获取电机状态
形    参：Channel：通道
*********************************************************/
void TMC5240_QueryMotorStatus(uint8_t Channel)
{
	switch(Channel)
	{
		case 1://静止后的等待时间，防止速度突变
			Channel1_MotorStatus.CHOPCONF=TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF,1);//获取电机数据
			Channel1_MotorStatus.DRV_STATUS=TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS,1);//获取电机数据
			Channel1_MotorStatus.GSTAT=TMC5240_ReadRegister(Channel,TMC5240_GSTAT,1);//获取电机数据
			Channel1_MotorStatus.RAMP_STAT=TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT,1);//获取电机数据
		
			if(	((Channel1_MotorStatus.RAMP_STAT&0x00000200)!=0)//到达目标位置
					//||((Channel1_MotorStatus.RAMP_STAT&0x00000003)!=0)//左右开关挂起
					||((Channel1_MotorStatus.RAMP_STAT&0x00000400)!=0))//实际速度为0
				Channel1_MotorStatus.Busy=0;//电机不忙
		
			break;
		
		case 2://静止后的等待时间，防止速度突变
			Channel2_MotorStatus.CHOPCONF=TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF,1);//获取电机数据
			Channel2_MotorStatus.DRV_STATUS=TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS,1);//获取电机数据
			Channel2_MotorStatus.GSTAT=TMC5240_ReadRegister(Channel,TMC5240_GSTAT,1);//获取电机数据
			Channel2_MotorStatus.RAMP_STAT=TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT,1);//获取电机数据
				
			if(	((Channel2_MotorStatus.RAMP_STAT&0x00000200)!=0)//到达目标位置
					//||((Channel2_MotorStatus.RAMP_STAT&0x00000003)!=0)//左右开关挂起
					||((Channel2_MotorStatus.RAMP_STAT&0x00000400)!=0))//实际速度为0
					Channel2_MotorStatus.Busy=0;//电机不忙
			
			break;
		
		case 3://静止后的等待时间，防止速度突变
			Channel3_MotorStatus.CHOPCONF=TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF,1);//获取电机数据
			Channel3_MotorStatus.DRV_STATUS=TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS,1);//获取电机数据
			Channel3_MotorStatus.GSTAT=TMC5240_ReadRegister(Channel,TMC5240_GSTAT,1);//获取电机数据
			Channel3_MotorStatus.RAMP_STAT=TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT,1);//获取电机数据
			
			if(	((Channel3_MotorStatus.RAMP_STAT&0x00000200)!=0)//到达目标位置
					//||((Channel3_MotorStatus.RAMP_STAT&0x00000003)!=0)//左右开关挂起
					||((Channel3_MotorStatus.RAMP_STAT&0x00000400)!=0))//实际速度为0
				Channel3_MotorStatus.Busy=0;//电机不忙
			
			break;
		
		case 4://静止后的等待时间，防止速度突变
			Channel4_MotorStatus.CHOPCONF=TMC5240_ReadRegister(Channel,TMC5240_CHOPCONF,1);//获取电机数据
			Channel4_MotorStatus.DRV_STATUS=TMC5240_ReadRegister(Channel,TMC5240_DRVSTATUS,1);//获取电机数据
			Channel4_MotorStatus.GSTAT=TMC5240_ReadRegister(Channel,TMC5240_GSTAT,1);//获取电机数据
			Channel4_MotorStatus.RAMP_STAT=TMC5240_ReadRegister(Channel,TMC5240_RAMPSTAT,1);//获取电机数据
			
			if(	((Channel4_MotorStatus.RAMP_STAT&0x00000200)!=0)//到达目标位置
					//||((Channel4_MotorStatus.RAMP_STAT&0x00000003)!=0)//左右开关挂起
					||((Channel4_MotorStatus.RAMP_STAT&0x00000400)!=0))//实际速度为0
				Channel4_MotorStatus.Busy=0;//电机不忙
			
			break;
		
		default:
			break;
	}
}

/*********************************************************
函数名称：TMC5240_QueryMotorStatus(uint8_t Channel)
描    述：获取电机状态
形    参：Channel：通道
*********************************************************/
void DataToBuffer(uint32_t Data)
{
//	RS485_UART_RXBuffer[5]=Data>>24;			
//	RS485_UART_RXBuffer[6]=Data>>16;
//	RS485_UART_RXBuffer[7]=Data>>8;
//	RS485_UART_RXBuffer[8]=Data;
	return;
}
