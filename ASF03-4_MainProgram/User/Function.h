#ifndef _FUNCTION_H
#define _FUNCTION_H
#include "HEAD.h"

extern uint8_t WorkMode;								//工作模式，1单次加液模式，2间隔加液模式，3持续加液模式。默认是单次加液模式
extern uint8_t Emergency_Stop_Flag;			//急停标志位
/*****************************************************************/
//校准相关
extern float 		Pump_Theoretically[4];	//校准模式预计加液量，默认7000；
extern float 		Pump_Actually[4];				//校准模式实际加液量。
extern float 		Pump_OneStep[4];				//单步加液量

extern uint8_t ScreenState;					//当前屏幕状态，0在工作界面，1在校准界面。
/*****************************************************************/
//加液相关
extern uint8_t 	TitleFlag[4];				//提示
extern uint8_t 	SelctedChannel[4];	//选中标志位
extern float 		Add_Volume[4];			//加液量


extern uint32_t Add_Times;								//加液次数，循环模式
extern uint32_t Add_IntervalTime;					//加液间隔时间，循环模式
extern uint8_t  Add_IntervalTimeFlag;			//加液间隔时间标志位,用定时器三去定时，定时器3为10ms
extern uint32_t Add_IntervalTimeCounter;	//加液间隔定时器计数

extern float 		ALLPumpSpeed;							//蠕动泵的转速
extern uint8_t 	ALLDirFlag;								//方向标志位

extern uint8_t 	MethodHide_Flag;			//单标方法选择菜单栏弹出标志位
/*****************************************************************/
//校准相关
extern uint8_t 	CaSelctedChannel[4];	//选中标志位


/*****************************************************************/
extern float Pump_BackDraw;//蠕动泵回吸容量，单位mL
extern float Pump_FillPipe;//蠕动泵填充管道容量，单位mL
extern float Pump_ClearPipe;//清洗管道容量，单位mL
/*****************************************************************/
//后台参数结构体
typedef struct
{
	__IO uint32_t  	Irun;   			/* 电机运行电流 */
	__IO uint32_t  	Ihold; 				/* 电机保持电流 */
	__IO uint8_t		CoolStep;			/* 电机节能模式 */

//	uint8_t					FristPowerOnFlag;	
//	uint8_t					ALLDirFlag;
//	float 					ALLPumpSpeed;
}SettingParametersSTR;

extern SettingParametersSTR SettingParameters;		//后台参数

typedef struct
{
	char  		Name[40];   				/* 方法名称 */
	uint8_t 	TitleFlag[4];				/* 提示 */
	uint8_t 	SelctedChannel[4];	/* 选中标志位 */
	float 		Add_Volume[4];			/* 加液量 */
	uint8_t   WorkMode;						/* 运行模式 */
	uint32_t  Add_Times;					/* 加液次数 */
	uint32_t  Add_IntervalTime;		/* 加液间隔时间 */
}MethodSTR;

extern MethodSTR 		Method;				//保存方法
extern uint32_t  		MethodNum;		//方法数量
extern uint8_t 			Method_Name_Unput_Flag;	//方法名称录入有效标志位
extern uint8_t 			Method_Unput_Flag;			//方法管理界面方法名弹出有效标志位
extern uint8_t      Method_SwitchFlag;			//方法管理界面选中的方法的位次
/*****************************************************************/
//功能函数
void FillPipe(void);								//填充管道功能
void ClearPipe(void);								//清洗管道功能
void BackDraw(void);								//回吸，清除管道内的液体
void Emergency_Stop(void);					//急停程序
void Clear_TFT_USART_RX_Buf(void);	//清空接收BUFF
	
void WorkMode_Single(void);					//单次加液模式
void WorkMode_Interval(void);				//间隔加液模式
void WorkMode_Continued(void);			//持续加液模式

int ASCIItoDEC(uint8_t *data);
float ASCIItoFloat(uint8_t *Arry);
void Flag_Init(void);
void ClearMororTSpeedFlag(void);
void ReadMethod_Show(void);
void ReadMethodManage_Show(uint8_t Method_Num);
#endif
