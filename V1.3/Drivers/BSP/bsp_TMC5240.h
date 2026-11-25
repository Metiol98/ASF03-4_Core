#ifndef __BSP_TCM5240_H
#define __BSP_TCM5240_H

#include "main.h"

#define TMC5240_SPI			hspi1

/* 片选引脚 */
//CS1
#define TMC5240_SPI_CS1_OFF				HAL_GPIO_WritePin(SPI_CS1_GPIO_Port,SPI_CS1_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_CS1_ON				HAL_GPIO_WritePin(SPI_CS1_GPIO_Port,SPI_CS1_Pin,GPIO_PIN_RESET)
//CS2
#define TMC5240_SPI_CS2_OFF				HAL_GPIO_WritePin(SPI_CS2_GPIO_Port,SPI_CS2_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_CS2_ON				HAL_GPIO_WritePin(SPI_CS2_GPIO_Port,SPI_CS2_Pin,GPIO_PIN_RESET)
//CS3
#define TMC5240_SPI_CS3_OFF				HAL_GPIO_WritePin(SPI_CS3_GPIO_Port,SPI_CS3_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_CS3_ON				HAL_GPIO_WritePin(SPI_CS3_GPIO_Port,SPI_CS3_Pin,GPIO_PIN_RESET)
//CS4
#define TMC5240_SPI_CS4_OFF				HAL_GPIO_WritePin(SPI_CS4_GPIO_Port,SPI_CS4_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_CS4_ON				HAL_GPIO_WritePin(SPI_CS4_GPIO_Port,SPI_CS4_Pin,GPIO_PIN_RESET)

/* 使能引脚 */
//EN1
#define TMC5240_SPI_EN1_OFF				HAL_GPIO_WritePin(EN1_GPIO_Port,EN1_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_EN1_ON					HAL_GPIO_WritePin(EN1_GPIO_Port,EN1_Pin,GPIO_PIN_RESET)
//EN2
#define TMC5240_SPI_EN2_OFF				HAL_GPIO_WritePin(EN2_GPIO_Port,EN2_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_EN2_ON					HAL_GPIO_WritePin(EN2_GPIO_Port,EN2_Pin,GPIO_PIN_RESET)
//EN3
#define TMC5240_SPI_EN3_OFF				HAL_GPIO_WritePin(EN3_GPIO_Port,EN3_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_EN3_ON					HAL_GPIO_WritePin(EN3_GPIO_Port,EN3_Pin,GPIO_PIN_RESET)
//EN4
#define TMC5240_SPI_EN4_OFF				HAL_GPIO_WritePin(EN4_GPIO_Port,EN4_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_EN4_ON					HAL_GPIO_WritePin(EN4_GPIO_Port,EN4_Pin,GPIO_PIN_RESET)

/* 参考引脚 */
#define TMC5240_SPI_REFR_OFF			HAL_GPIO_WritePin(REFR1_GPIO_Port,REFR1_Pin,GPIO_PIN_RESET)
#define TMC5240_SPI_REFR_ON				HAL_GPIO_WritePin(REFR1_GPIO_Port,REFR1_Pin,GPIO_PIN_SET)
#define TMC5240_SPI_REFL_OFF			HAL_GPIO_WritePin(REFL1_GPIO_Port,REFL1_Pin,GPIO_PIN_RESET)
#define TMC5240_SPI_REFL_ON				HAL_GPIO_WritePin(REFL1_GPIO_Port,REFL1_Pin,GPIO_PIN_SET)

//READ or Write 读写标志位
#define TMC_WRITE 	1
#define TMC_READ 		0


typedef struct _COMMUNICATION_REGISTER{
    
    uint8_t RS          :7 ;     // Reg Address
    uint8_t WEN         :1 ;     // Write Enable Bit
    
}CRDDATA_Typdef;

//READ or Write 读写标志位
#define TMC_WRITE 	1
#define TMC_READ 		0


//5240寄存器地址
#define TMC5240_GCONF          0x00
#define TMC5240_GSTAT          0x01
#define TMC5240_IFCNT          0x02
#define TMC5240_SLAVECONF      0x03
#define TMC5240_INP_OUT        0x04
#define TMC5240_X_COMPARE      0x05
#define TMC5240_OTP_PROG       0x06
#define TMC5240_OTP_READ       0x07
#define TMC5240_FACTORY_CONF   0x08
#define TMC5240_SHORT_CONF     0x09
#define TMC5240_DRV_CONF       0x0A
#define TMC5240_GLOBAL_SCALER  0x0B
#define TMC5240_OFFSET_READ    0x0C
#define TMC5240_IHOLD_IRUN     0x10
#define TMC5240_TPOWERDOWN     0x11
#define TMC5240_TSTEP          0x12
#define TMC5240_TPWMTHRS       0x13
#define TMC5240_TCOOLTHRS      0x14
#define TMC5240_THIGH          0x15

#define TMC5240_RAMPMODE       0x20				//PAGE35: 0--位置模式 1--速度模式（到正VMAX,使用AMAX加速度）	2--速度模式（到负VMAX,使用AMAX加速度）	3--保持模式
#define TMC5240_XACTUAL        0x21
#define TMC5240_VACTUAL        0x22
#define TMC5240_VSTART         0x23
#define TMC5240_A1             0x24
#define TMC5240_V1             0x25
#define TMC5240_A2             0x2F
#define TMC5240_V2             0x2E
#define TMC5240_AMAX           0x26
#define TMC5240_VMAX           0x27
#define TMC5240_DMAX           0x28
#define TMC5240_D2           	 0x30
#define TMC5240_D1             0x2A
#define TMC5240_VSTOP          0x2B
#define TMC5240_TZEROWAIT      0x2C
#define TMC5240_XTARGET        0x2D

#define TMC5240_VDCMIN         0x33
#define TMC5240_SWMODE         0x34				//PAGE38：位11：0--硬停 1--软停	 位1：使能右参考开关使能时电机停止，释放启动  位0：使能左参考开关使能时电机停止，释放启动
#define TMC5240_RAMPSTAT       0x35
#define TMC5240_XLATCH         0x36
#define TMC5240_ENCMODE        0x38
#define TMC5240_XENC           0x39
#define TMC5240_ENC_CONST      0x3A
#define TMC5240_ENC_STATUS     0x3B
#define TMC5240_ENC_LATCH      0x3C
#define TMC5240_ENC_DEVIATION  0x3D

#define TMC5240_MSLUT0         0x60
#define TMC5240_MSLUT1         0x61
#define TMC5240_MSLUT2         0x62
#define TMC5240_MSLUT3         0x63
#define TMC5240_MSLUT4         0x64
#define TMC5240_MSLUT5         0x65
#define TMC5240_MSLUT6         0x66
#define TMC5240_MSLUT7         0x67
#define TMC5240_MSLUTSEL       0x68
#define TMC5240_MSLUTSTART     0x69
#define TMC5240_MSCNT          0x6A
#define TMC5240_MSCURACT       0x6B
#define TMC5240_CHOPCONF       0x6C
#define TMC5240_COOLCONF       0x6D
#define TMC5240_DCCTRL         0x6E
#define TMC5240_DRVSTATUS      0x6F
#define TMC5240_PWMCONF        0x70
#define TMC5240_PWMSCALE       0x71
#define TMC5240_PWM_AUTO       0x72
#define TMC5240_LOST_STEPS     0x73
#define TMC5240_SG4_THRS     	 0x74
#define TMC5240_SG4_RESULT     0x75
#define TMC5240_SG4_IND     	 0x76



/* 电机实时状态结构体，在电机运行的过程中查询 */
typedef struct
{
	__IO uint32_t		CHOPCONF;			/* 斩波器配置 */
	__IO uint32_t  	QueryCNT;			/* 电机状体查询计数 */
	__IO uint8_t  	Busy;					/* 电机忙标志位 */
	__IO uint8_t  	Dir;					/* 电机方向 */
	__IO uint32_t  	RAMP_STAT;    /* 斜坡与参考开关 */
	__IO uint32_t  	DRV_STATUS;   /* 驱动错误标志位 */
	__IO uint32_t  	GSTAT;    		/* 全局标志位 */
	__IO uint32_t  	PWMSCALE;    	/* 全局标志位 */
	__IO uint32_t  	PWM_AUTO;    	/* 全局标志位 */
	__IO uint32_t  	XACTUAL;    	/* 实际电机位置 */
	__IO uint32_t  	VACTUAL;    	/* 实际电机速度 */
	float 					Position;			/* 电机绝对位置 */
} MotorStatusSTR;								/* 电机状态结构体 */

extern MotorStatusSTR	Channel1_MotorStatus;     /* 电机1状态结构体 */
extern MotorStatusSTR	Channel2_MotorStatus;     /* 电机2状态结构体 */
extern MotorStatusSTR	Channel3_MotorStatus;     /* 电机3状态结构体 */
extern MotorStatusSTR	Channel4_MotorStatus;     /* 电机4状态结构体 */

/* TMC5240读写寄存器函数 */
uint32_t TMC5240_WriteReadRegister(uint8_t Channel, uint8_t Regaddr, uint32_t Data, uint8_t RW_Flag);
uint32_t TMC5240_ReadRegister(uint32_t Channel, uint8_t Regaddr);

/* TMC5240初始化函数 */
void TMC5240_init(void);
void TMC5240_SetFirstState(uint8_t Channel);

/* TMC5240基础功能函数 */
void TMC5240_ClearPosition(uint8_t Channel);
void TMC5240_Rotate(uint8_t Channel, int32_t Speed);
void TMC5240_MoveTo(uint8_t Channel, int32_t position);
void TMC5240_Stop(uint8_t Channel);
void TMC5240_StopNow(uint8_t Channel);

/* TMC5240参数设置函数 */
void TMC5240_SetSpeed(uint8_t Channel, float Speed);
void TMC5240_SetIRUN(uint8_t Channel, uint8_t IRUN);
void TMC5240_SetIHOLD(uint8_t Channel, uint8_t IHOLD);
void TMC5240_SetMotionAP(uint8_t Channel, int8_t Regaddr,int32_t Data);

/* TMC5240查询电机状态函数 */
void TMC5240_QueryMotorStatus(uint8_t Channel);
	
#endif
