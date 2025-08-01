#ifndef _BSP_PUMP_H
#define _BSP_PUMP_H
#include "HEAD.h"

/************************************************************************************/
//Pump1 TIM2 输出通道1  PA0
#define  	Pump_1_TIM                   	TIM2
#define   Pump_1_TIM_APBxClockCmd     	RCC_APB1PeriphClockCmd
#define   Pump_1_TIM_CLK               	RCC_APB1Periph_TIM2
#define   Pump_1_TIM_OCxInit           	TIM_OC1Init							//通道初始化
#define   Pump_1_TIM_OCxPreloadConfig  	TIM_OC1PreloadConfig		//通道重装载
// 输出PWM的频率为 72M/{ (ARR+1)*(PSC+1) }
#define   Pump_1_TIM_PERIOD            	65535
#define   Pump_1_TIM_PSC               	(9-1)
#define   Pump_1_TIM_PULSE             	0
//中断相关
#define   Pump_1_TIM_IRQn             	TIM2_IRQn
#define   Pump_1_TIM_IRQHandler        	TIM2_IRQHandler

#define   Pump_1_GPIO_APBxClockCmd     	RCC_APB2PeriphClockCmd

#define 	Pump_1_TIM_PWM_GPIO_CLK      	RCC_APB2Periph_GPIOA
#define  	Pump_1_TIM_PWM_PORT          	GPIOA
#define  	Pump_1_TIM_PWM_PIN           	GPIO_Pin_0

#define 	Pump_1_TIM_EN_GPIO_CLK      	RCC_APB2Periph_GPIOB
#define  	Pump_1_TIM_EN_PORT          	GPIOB
#define  	Pump_1_TIM_EN_PIN          		GPIO_Pin_2

#define 	Pump_1_TIM_DIR_GPIO_CLK      	RCC_APB2Periph_GPIOA
#define  	Pump_1_TIM_DIR_PORT          	GPIOA
#define  	Pump_1_TIM_DIR_PIN           	GPIO_Pin_2
/************************************************************************************/
//使能/方向控制
#define Pump_1_EN_Enable				GPIO_SetBits(Pump_1_TIM_EN_PORT,Pump_1_TIM_EN_PIN)
#define Pump_1_EN_Disable				GPIO_ResetBits(Pump_1_TIM_EN_PORT,Pump_1_TIM_EN_PIN)
#define Pump_1_DIR_Forword			GPIO_SetBits(Pump_1_TIM_DIR_PORT,Pump_1_TIM_DIR_PIN)
#define Pump_1_DIR_Back					GPIO_ResetBits(Pump_1_TIM_DIR_PORT,Pump_1_TIM_DIR_PIN)

/************************************************************************************/
//泵管流量
#define Pump_Pipe_Rate		933			//16#泵管流量，uL/r，微升每转

//蠕动泵细分值
#define Pump_1_DIVision		16

//蠕动泵占空比
#define Pump_1_Pulse			50

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*频率相关参数*/
#define TIM_FREQ            72000000                      	/* 定时器主频 */
#define MAX_STEP_ANGLE      (float)1.8/MICRO_STEP           /* 最小步距(1.8/MICRO_STEP) */
#define PAI                 3.1415926                       /* 圆周率*/
#define FSPR                200                             /* 步进电机单圈步数 */
#define MICRO_STEP          16                              /* 步进电机驱动器细分数 */
#define T1_FREQ             (TIM_FREQ/(Pump_1_TIM_PSC+1)) /* 频率ft值 */
#define SPR                 (FSPR*MICRO_STEP)               /* 旋转一圈需要的脉冲数 */


/* 数学常数 */
#define ALPHA               ((float)(2*PAI/SPR))            /* α = 2*pi/spr */
#define A_T_x10             ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148         ((float)((T1_FREQ*0.69)/10))    /* 0.69为误差修正值 */
#define A_SQ                ((float)(2*100000*ALPHA))
#define A_x200              ((float)(200*ALPHA))            /* 2*10*10*a/10 */

/*电机速度决策中的四个状态*/
#define STOP              0 // 停止状态
#define ACCEL             1 // 加速状态
#define DECEL             2 // 减速状态
#define RUN               3 // 匀速状态

#define CW  							1  //顺时针
#define CCW 							0  //逆时针

#define ACCEL_VAL					(uint32_t)(1000.0*(2.0-0.9549))//accel  加速度,实际值为accel*0.1*rad/sec^2  10倍并且2个脉冲算一个完整的周期
#define DECEL_VAL					(uint32_t)(1000.0*(2.0-0.9549))//decel  减速度,实际值为decel*0.1*rad/sec^2

typedef struct
{
    __IO uint8_t  run_state;                                /* 电机旋转状态 */
    __IO uint8_t  dir;                                      /* 电机旋转方向 */
    __IO int32_t  step_delay;                               /* 下个脉冲周期（时间间隔），启动时为加速度 */
    __IO uint32_t decel_start;                              /* 开始减速位置 */
    __IO int32_t  decel_val;                                /* 减速阶段步数 */
    __IO int32_t  min_delay;                                /* 速度最快，计数值最小的值(最大速度，即匀速段速度) */
    __IO int32_t  accel_count;                              /* 加减速阶段计数值 */
} speedRampData;


extern speedRampData g_srd;
extern uint8_t  g_motion_sta;                    /* 是否在运动？0：停止，1：运动 */


extern uint16_t Pump_last_accel_delay;              /* 加速过程中最后一次延时（脉冲周期） */
extern uint32_t Pump_step_count;                    /* 总移动步数计数器*/
extern int32_t Pump_rest;                           /* 记录new_step_delay中的余数，提高下一步计算的精度 */
extern uint8_t Pump_i;       



void create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Speed_Decision_param(void);
/************************************************************************************/
extern uint32_t SpeedUp_Down_Count;//加速标志
extern uint32_t Pump_1_IRQn_CNT;//蠕动泵中断函数计数标志位
extern uint32_t Pump_1_PulseNum;//蠕动泵脉冲个数存储位
extern uint8_t Pump_1_RunTimeSaveFlag;

void Pump_1_Init(void);

void Pump_Speed_Set(float Speed);
void Pump_Motion(int Dir,float Liq);
#endif

