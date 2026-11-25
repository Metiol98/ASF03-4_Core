#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"

#define	MODESUM									3
#define SINGLE_MODE 						1	 	/* 单次加液 */
#define INTERVAL_MODE 					2		/* 循环加液 */
#define CONTINUE_MODE 					3		/* 持续加液 */

#define USER_BUSY   		1
#define USER_FREE   		(!USER_BUSY)

#define USER_DISPLAY   	1
#define USER_HIDE  			(!USER_DISPLAY)

#define USER_ENABLE 				1								/* 使能 */
#define USER_DISABLE 				(!USER_ENABLE) 	/* 失能 */

#define USER_TRUE						1								/* 真 */
#define USER_FALSE					(!USER_TRUE)		/* 假 */

extern uint8_t MethodHide_Flag;

extern volatile uint8_t WorkMode;

/**
  * @brief  屏幕页数枚举
  */
typedef enum
{
	Screen_Working = 0,
	Screen_Setting,
	Screen_Parameter,
	Screen_SRX885Pair,
	Screen_PumpTubeManagement,
	Screen_PumpTubeAlert,
	Screen_PumpTubeRegistration
}ScreenPage;

/**
  * @brief  参数配置结构体，需要保存的后台数据
  */
typedef struct
{
	uint8_t FristPowerOnInit;		/* 上电初始化标志位 */

	float PipeFillVolume[4]; 			/* 蠕动泵填充管道容量，单位mL */
	float PipeClearVolume[4];			/* 蠕动泵清洗管道容量，单位mL */
	float PipeBackDrawVolume[4];		/* 蠕动泵回吸管道容量，单位mL */

	float PumpOneStep[4];			/* 单步进样量 */
	float PumpSpeed[4];					/* 蠕动泵的转速 */
	uint8_t ALLPumpDir;					/* 电机运行方向 */

	char TitleName[4][60];				/* 提示 */

	float AddVolume[4];				/* 加液量 */

	__IO uint32_t  	Irun;   			/* 电机运行电流 */
	__IO uint32_t  	Ihold; 				/* 电机保持电流 */
	__IO uint8_t		CoolStep;			/* 电机节能模式 */

	__IO uint32_t		PipeCallBackVolume[4];	/* 蠕动泵回调管道容量，单位uL */
}MCBConfigSTR;
extern MCBConfigSTR g_MCBConfig;

/**
  * @brief  系统当前状态结构体
  */
typedef struct
{
	uint8_t 		WorkingBusy;		/* 0表示空闲，1表示忙碌 */
	uint8_t 		EmergencyStop; 	/* 急停标志位 */
	ScreenPage 	NowScreenPage; 	/* 屏幕页码 */

	uint8_t 		SelectedChannel[4];	/* 选中标志位 */
	uint8_t			CaSelctedChannel[4];/* 校准界面选中标志位 */

	uint32_t 		SRX885Counter;	/* 手柄按键计数，防止按键粘连 */
	uint8_t 		SRX885OK;				/* 手柄按键有效 */
	uint8_t 		SRX885PairMode;	/* 手柄当前是否在对码模式下 */

	uint8_t 		SaveMethodPressDropMenu;				//方法保存界面下拉菜单按下标志位
	uint8_t			SaveMethodDropMenuChooseNum;		//方法保存界面下拉菜单选择的序号
	
	uint8_t 		ManageMethodPressDropMenu;			//方法管理界面下拉菜单按下标志位
	uint8_t 		ManageMethodDropMenuChooseNum;	//方法保存界面下拉菜单选择的序号
	uint8_t 		ManageMethodPressOK;						//方法界面是否有选择（防止未选择序号情况下点击删除）
}NowStatusSTR;
extern NowStatusSTR g_NowStatus;


/**
  * @brief  间隔加液模式
  */
typedef struct
{
	float 		IntervalTime;		/* 循环加液模式加液间隔时间 */
	uint32_t 	AddNumber;			/* 循环加液模式加液次数 */
	uint32_t 	InterTimeCNT;		/* 循环加液模式加液间隔时间计数 */
	uint8_t	 	InterTimeOver;	/* 间隔时间到 */
}IntervalSTR;
extern IntervalSTR g_Interval;

/**
  * @brief  校准模式
  */
typedef struct
{
	float Pump_Theoretically[4];	/* 校准模式预计加液量，默认7000uL */
	double Pump_Actually[4];	/* 校准模式实际加液量，uL */
} CaliSTR;
extern CaliSTR g_Cali;

/**
  * @brief  外部电源相关参数结构体
  */
typedef struct
{
	uint8_t	 	FirstPowerOn;	/* 首次上电标志位 */
	uint8_t 	OutPowerFlag;	/* 外部电源接入标志位 */
	float 		AVG;					/* 采集电量均值 */
	uint32_t 	TimCNT;				/* 电源定时器计数 500ms */
	uint8_t 	IconBlink;		/* 电源图标闪烁标志位 */
	uint8_t 	IconBlinkCNT;	/* 电源图标闪烁计数 */
}BatterySTR;
extern BatterySTR g_Battery;


#define MAX_METHOD_COUNT			10	/* 最多可以存储10个 */
/**
  * @brief  加液方法保存结构体
  */
typedef struct
{
	char  		Name[60];   		/* 方法名称 */
	char 		TitleName[4][60];	/* 提示 */
	uint8_t 	SelctedChannel[4];	/* 选中标志位 */
	float 		Add_Volume[4];		/* 加液量 */
	uint8_t		Mode;				//工作模式保存
	uint32_t  	Add_Times;			/* 加液次数 */
	float	  	Add_IntervalTime;	/* 加液间隔时间 */
	uint8_t 	valid;				//方法是否有效（0-无效，1-有效）
} SaveMethodSTR;
extern SaveMethodSTR SaveMethod[MAX_METHOD_COUNT];

#define MAX_RECORD_COUNT			100	/* 最多可以存储100条记录 */
/**
  * @brief  加液记录存储保存结构体
  */
typedef struct
{
	uint8_t 		Mode;							/* 工作模式保存 */
	uint32_t  		Add_Times;				/* 加液次数 */
	float  		Add_IntervalTime;	/* 加液间隔时间 */
	float 			Add_Volume[4];		/* 加液量 */
	uint8_t 		SelctedChannel[4];/* 选中标志位 */
	
}SaveRecordSTR;

/**
  * @brief  待执行指令结构体，从屏幕和485环形队列取出指令准备执行。
  */
typedef struct
{
  uint32_t  ScreenID;         /* 屏幕ID */
  uint32_t  ButtonID;         /* 控件ID */
  uint32_t 	CMDLen;           /* 指令长度 */
  uint8_t   ReadOutArry[30];  /* 新指令标志位 */
  uint8_t   PendingCMDFlag;   /* 指令待处理标志位 */
}PendingCMDSTR;
extern PendingCMDSTR g_TFTPendingCMD; 

void System_Init(void);
void Flag_Init(void);

void WorkMode_Single(void);					 /* 单次加液模式 */
void WorkMode_Interval(void);				 /* 间隔加液模式 */
void WorkMode_Continued(void);			 /* 持续加液模式 */

void ClearPipe(void);
void BackSuck(void);
void FillPipe(void);

void Method_RefreshDisplay(void);


void Emergency_Stop(void);
void EmergencyStopJudge(uint8_t *Arry);

#endif

