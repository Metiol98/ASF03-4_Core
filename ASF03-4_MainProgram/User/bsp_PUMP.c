#include "HEAD.h"

/*****************************************************************
* 名    称： Pump_1_NVIC_Config()
* 功    能： 屏幕串口优先级分配
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void Pump_1_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructur;
	//USART1,分组0，抢占0，子优先级14
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);									//分组
	NVIC_InitStructur.NVIC_IRQChannel=Pump_1_TIM_IRQn; 							//通道
	NVIC_InitStructur.NVIC_IRQChannelPreemptionPriority=0;					//抢占优先级，主优先级,查看分组表；
	NVIC_InitStructur.NVIC_IRQChannelSubPriority =1;								//子优先级；
	NVIC_InitStructur.NVIC_IRQChannelCmd =ENABLE;										//开使能；
	NVIC_Init(&NVIC_InitStructur);																	//NVIC初始化；
}
/*****************************************************************
* 名    称： Pump_1_GPIO_Config()
* 功    能： Pump_1GPIO初始化
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void Pump_1_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	Pump_1_GPIO_APBxClockCmd(		Pump_1_TIM_PWM_GPIO_CLK|
															Pump_1_TIM_EN_GPIO_CLK|
															Pump_1_TIM_DIR_GPIO_CLK, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin =  Pump_1_TIM_PWM_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(Pump_1_TIM_PWM_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  Pump_1_TIM_EN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(Pump_1_TIM_EN_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  Pump_1_TIM_DIR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(Pump_1_TIM_DIR_PORT, &GPIO_InitStructure);
	
	GPIO_ResetBits(Pump_1_TIM_EN_PORT,Pump_1_TIM_EN_PIN);//关闭使能
}
/*****************************************************************
* 名    称： Motor_X_TIM_Config()
* 功    能： Motor_X脉冲初始化
* 入口参数： 
* 出口参数： 
 *****************************************************************/
static void Pump_1_TIM_Config(void)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	Pump_1_TIM_APBxClockCmd(Pump_1_TIM_CLK,ENABLE);

	/*--------------------时基结构体初始化-------------------------*/	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period=Pump_1_TIM_PERIOD;				// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Prescaler= Pump_1_TIM_PSC;				// 驱动CNT计数器的时钟 = Fck_int/(psc+1)	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;				// 时钟分频因子 ，配置死区时间时需要用到，1分频	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;							// 重复计数器的值，没用到不用管

	TIM_TimeBaseInit(Pump_1_TIM, &TIM_TimeBaseStructure);			// 初始化定时器

	/*--------------------输出比较结构体初始化-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;//c99标准支持任意地方定义变量
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;								// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		// 输出使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 互补输出使能
	TIM_OCInitStructure.TIM_Pulse = Pump_1_TIM_PULSE;							// 设置占空比大小，占空比P=CCR/(ARR+1)=4/8=50%
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				// 输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			// 互补输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;			// 输出通道空闲电平极性配置，刹车时的电平
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// 互补输出通道空闲电平极性配置，刹车时的电平

	Pump_1_TIM_OCxInit(Pump_1_TIM, &TIM_OCInitStructure);						//初始化通道
	Pump_1_TIM_OCxPreloadConfig(Pump_1_TIM, TIM_OCPreload_Disable);	//这里设置为Disable就是为了后面在中断服务子程序可以修改TIMx_CCR实时起作用~

//	TIM_ClearFlag(Pump_1_TIM, TIM_FLAG_CC1);											//清除计数器中断标志位
	TIM_ITConfig(Pump_1_TIM, TIM_IT_CC1, ENABLE);									//开启计数器中断

//使能重载寄存器ARR  
//	TIM_ARRPreloadConfig(Pump_1_TIM, ENABLE);
// 使能计数器
	TIM_Cmd(Pump_1_TIM, DISABLE);
	
// 主输出使能，当使用的是通用定时器时，这句不需要
//	TIM_CtrlPWMOutputs(AirControl_TIM, ENABLE);
}


void Pump_1_Init(void)
{
	Pump_1_NVIC_Config();
	Pump_1_GPIO_Config(); 
	Pump_1_TIM_Config();
}


/*****************************************************************
* 名    称： Pump_1_TIM_IRQHandler()
* 功    能： 蠕动泵中断函数
* 入口参数： 
* 出口参数： 
 *****************************************************************/
uint32_t Pump_1_IRQn_CNT=0;//蠕动泵中断函数计数标志位
uint32_t Pump_1_PulseNum=0;//蠕动泵脉冲个数存储位
uint8_t Pump_1_RunTimeSaveFlag=0;//可以保存标志位，保存时间太长不能放在中断里，把保存放到while大循环
uint32_t SpeedUp_Down_Count=20;//加速标志
void Pump_1_TIM_IRQHandler(void) 
{ 
	if(TIM_GetITStatus(Pump_1_TIM,TIM_IT_CC1)!=RESET) 
	{ 
		TIM_ClearITPendingBit(Pump_1_TIM,TIM_IT_CC1);
		Speed_Decision_param();
	}
} 


/*********************************************************************************************************************************************/
/*********************************************************************************************************************************************/
/*********************************************************************************************************************************************/
/*********************************************************************************************************************************************/


/*****************************************************************
* 名    称：Pump_Speed_Set(float Speed)  
* 功    能：蠕动泵转速\占空比\细分值设置
* 入口参数：
						Speed：转速，r/min
						StepNum：电机细分值
						Pulse:占空比，0-100取值
* 出口参数： 
 *****************************************************************/
uint32_t speed_val=0;
void Pump_Speed_Set(float Speed)    
{  
//	Pump_1_TIM->ARR=1000000.0/((Speed/60.0)*200.0*StepNum)-1;//Speed*(1350000/Speed)-1;  //自动重装载寄存器的值
//	Pump_1_TIM->CCR4 =(1000000.0/((Speed/60.0)*200.0*StepNum))*(Pulse/100.0);  //占空比
//	TIM_Cmd(Pump_1_TIM, DISABLE);//关闭使能；
//	uint32_t accel_val=1000.0*(2.0-0.9549);//accel  加速度,实际值为accel*0.1*rad/sec^2  10倍并且2个脉冲算一个完整的周期
//	uint32_t decel_val=1000.0*(2.0-0.9549);//decel  减速度,实际值为decel*0.1*rad/sec^2
	speed_val=Speed*(2.0-0.9549);//speed  最大速度,实际值为speed*0.1*rad/sec
}	 


/*****************************************************************
* 名    称： Pump_Motion(int Dir,float Liq)
* 功    能： 蠕动泵动作函数，包括动作距离和方向
* 入口参数： 
						 Dir：蠕动泵加液方向
						 Liq：蠕动泵加液体积mL(范围0-100mL)
* 出口参数： 
*****************************************************************/
void Pump_Motion(int Dir,float Liq)
{
	if(Dir==1)									//判断方向
		Pump_1_DIR_Forword;
	if(Dir==0)
		Pump_1_DIR_Back;
	
	Pump_1_PulseNum=(Liq*1000.0*1000.0/Flash_PumpOneStep)*(float)Pump_1_DIVision;//计步
	Pump_1_EN_Enable;//打开使能
	g_motion_sta = STOP;
	g_srd.run_state = STOP;
	create_t_ctrl_param(Pump_1_PulseNum, ACCEL_VAL, DECEL_VAL, speed_val);
	
//	Pump_1_WorkFinish=0;//蠕动泵完成标志位置0，表示待完成，完成后置1
//	Pump_1_IRQn_CNT=0;//计步标志位清零
//	SpeedUp_Down_Count=20;	//加速标志复位
//	Pump_1_EN_Enable;//打开使能
//	TIM_Cmd(Pump_1_TIM, ENABLE);//使能PWM输出
}


/********************************************梯形加减速***********************************************/
speedRampData g_srd               = {STOP,CW,0,0,0,0,0};  /* 加减速变量 */
__IO int32_t  g_step_position     = 0;                    /* 当前位置 */
uint8_t  g_motion_sta        = 0;                    /* 是否在运动？0：停止，1：运动 */
__IO uint32_t g_add_pulse_count   = 0;                    /* 脉冲个数累计 */

/*
 * @brief       生成梯形运动控制参数
 * @param       step：移动的步数 (正数为顺时针，负数为逆时针).
 * @param       accel  加速度,实际值为accel*0.1*rad/sec^2  10倍并且2个脉冲算一个完整的周期
 * @param       decel  减速度,实际值为decel*0.1*rad/sec^2
 * @param       speed  最大速度,实际值为speed*0.1*rad/sec
 * @retval      无
 */
void create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    __IO uint16_t tim_count;        /* 达到最大速度时的步数*/
    __IO uint32_t max_s_lim;        /* 必须要开始减速的步数（如果加速没有达到最大速度）*/
    __IO uint32_t accel_lim;
    if(g_motion_sta != STOP)        /* 只允许步进电机在停止的时候才继续*/
        return;
    if(step < 0)                    /* 步数为负数 */
    {   
        g_srd.dir = CCW;            /* 逆时针方向旋转 */
				//Pump_1_DIR_Back;		// 输出电机方向    
				//ST3_DIR(CCW);
        step = -step;               /* 获取步数绝对值 */
    }
    else
    {
        g_srd.dir = CW;             /* 顺时针方向旋转 */
				//Pump_1_DIR_Forword;	// 输出电机方向       
				//ST3_DIR(CW);
    }

    if(step == 1)                   /* 步数为1 */
    {
        g_srd.accel_count = -1;     /* 只移动一步 */
        g_srd.run_state = DECEL;    /* 减速状态. */
        g_srd.step_delay = 1000;    /* 默认速度 */
    }
    else if(step != 0)              /* 如果目标运动步数不为0*/
    {
        /*设置最大速度极限, 计算得到min_delay用于定时器的计数器的值 min_delay = (alpha / t)/ w*/
        g_srd.min_delay = (int32_t)(A_T_x10 /speed); //匀速运行时的计数值

        /* 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.1rad/sec^2
         step_delay = 1/tt * sqrt(2*alpha/accel)
         step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */
        
        g_srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10); /* c0 */

        max_s_lim = (uint32_t)(speed*speed / (A_x200*accel/10));/* 计算多少步之后达到最大速度的限制 max_s_lim = speed^2 / (2*alpha*accel) */

        if(max_s_lim == 0)                                      /* 如果达到最大速度小于0.5步，我们将四舍五入为0,但实际我们必须移动至少一步才能达到想要的速度 */
        { 
            max_s_lim = 1;
        }
        accel_lim = (uint32_t)(step*decel/(accel+decel));       /* 这里不限制最大速度 计算多少步之后我们必须开始减速 n1 = (n1+n2)decel / (accel + decel) */

        if(accel_lim == 0)                                      /* 不足一步 按一步处理*/
        {
            accel_lim = 1;
        }
        if(accel_lim <= max_s_lim)                              /* 加速阶段到不了最大速度就得减速。。。使用限制条件我们可以计算出减速阶段步数 */
        {
            g_srd.decel_val = accel_lim - step;                 /* 减速段的步数 */
        }
        else
        {
            g_srd.decel_val = -(max_s_lim*accel/decel);         /* 减速段的步数 */
        }
        if(g_srd.decel_val == 0)                                /* 不足一步 按一步处理 */
        {
            g_srd.decel_val = -1;
        }
        g_srd.decel_start = step + g_srd.decel_val;             /* 计算开始减速时的步数 */
        
        
        if(g_srd.step_delay <= g_srd.min_delay)                 /* 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速 */
        {
            g_srd.step_delay = g_srd.min_delay;
            g_srd.run_state = RUN;
        }
        else  
        {
            g_srd.run_state = ACCEL;
        }
        g_srd.accel_count = 0;                                  /* 复位加减速计数值 */
    }
    g_motion_sta = 1;                                           /* 电机为运动状态 */
    //ST3_EN(EN_ON);
    tim_count=TIM_GetCapture1(Pump_1_TIM);
		/*在当前计数值基础上设置定时器比较值*/
		Pump_1_TIM->CCR1 =tim_count+g_srd.step_delay;  //占空比

		/*使能定时器通道*/
		TIM_Cmd(Pump_1_TIM, ENABLE);
		TIM_CtrlPWMOutputs(Pump_1_TIM, ENABLE);                            /* 使能定时器通道 */
}


/**
  * @brief  定时器比较中断
  * @param  htim：定时器句柄指针
  * @note   无
  * @retval 无
  */
uint16_t Pump_last_accel_delay = 0;              /* 加速过程中最后一次延时（脉冲周期） */
uint32_t Pump_step_count = 0;                    /* 总移动步数计数器*/
int32_t Pump_rest = 0;                           /* 记录new_step_delay中的余数，提高下一步计算的精度 */
uint8_t Pump_i = 0;       
void Speed_Decision_param(void)
{
	__IO uint32_t tim_count = 0;
	__IO uint32_t tmp = 0;
	uint16_t new_step_delay = 0;                            /* 保存新（下）一个延时周期 */

	// 设置比较值
	tim_count=TIM_GetCapture1(Pump_1_TIM);
	tmp = tim_count+g_srd.step_delay/2;/* 整个C值里边是需要翻转两次的所以需要除以2 */
	Pump_1_TIM_OCxPreloadConfig(Pump_1_TIM, TIM_OCPreload_Disable);	//这里设置为Disable就是为了后面在中断服务子程序可以修改TIMx_CCR实时起作用~
	Pump_1_TIM->CCR1 =tmp; 
	
	Pump_i++;     // 定时器中断次数计数值
	if(Pump_i==2) // 2次，说明已经输出一个完整脉冲
	{
		Pump_i = 0;                                          /* 清零定时器中断次数计数值 */
		switch(g_srd.run_state)                         /* 加减速曲线阶段 */
		{
			case STOP:
				Pump_step_count = 0;                             /* 清零步数计数器 */
				Pump_rest = 0;                                   /* 清零余值 */
				/* 关闭通道*/
				TIM_Cmd(Pump_1_TIM, DISABLE);
				TIM_CtrlPWMOutputs(Pump_1_TIM, DISABLE);
				g_motion_sta = STOP;                           /* 电机为停止状态  */	
				break;

			case ACCEL:
				g_add_pulse_count++;                        /* 只用于记录相对位置转动了多少度 */
				Pump_step_count++;                               /* 步数加1*/
				if(g_srd.dir == CW)
				{
					g_step_position++;                      /* 绝对位置加1  记录绝对位置转动多少度*/
				}
				else
				{
					g_step_position--;                      /* 绝对位置减1*/
				}
				g_srd.accel_count++;                        /* 加速计数值加1*/
				new_step_delay = g_srd.step_delay - (((2 *g_srd.step_delay) + Pump_rest)/(4 * g_srd.accel_count + 1));/* 计算新(下)一步脉冲周期(时间间隔) */
				Pump_rest = ((2 * g_srd.step_delay)+Pump_rest)%(4 * g_srd.accel_count + 1);                                /* 计算余数，下次计算补上余数，减少误差 */
				if(Pump_step_count >= g_srd.decel_start)         /* 检查是否到了需要减速的步数 */
				{
					g_srd.accel_count = g_srd.decel_val;    /* 加速计数值为减速阶段计数值的初始值 */
					g_srd.run_state = DECEL;                /* 下个脉冲进入减速阶段 */
				}
				else if(new_step_delay <= g_srd.min_delay)  /* 检查是否到达期望的最大速度 计数值越小速度越快，当你的速度和最大速度相等或更快就进入匀速*/
				{
					Pump_last_accel_delay = new_step_delay;      /* 保存加速过程中最后一次延时（脉冲周期）*/
					new_step_delay = g_srd.min_delay;       /* 使用min_delay（对应最大速度speed）*/
					Pump_rest = 0;                               /* 清零余值 */
					g_srd.run_state = RUN;                  /* 设置为匀速运行状态 */
				}
				break;

			case RUN:
				g_add_pulse_count++;
				Pump_step_count++;                               /* 步数加1 */
				if(g_srd.dir == CW)
				{
					g_step_position++;                      /* 绝对位置加1 */
				}
				else
				{
					g_step_position--;                      /* 绝对位置减1*/
				}
				new_step_delay = g_srd.min_delay;           /* 使用min_delay（对应最大速度speed）*/
				if(Pump_step_count >= g_srd.decel_start)         /* 需要开始减速 */
				{
					g_srd.accel_count = g_srd.decel_val;    /* 减速步数做为加速计数值 */
					new_step_delay = Pump_last_accel_delay;      /* 加阶段最后的延时做为减速阶段的起始延时(脉冲周期) */
					g_srd.run_state = DECEL;                /* 状态改变为减速 */
				}
				break;

			case DECEL:
				Pump_step_count++;                               /* 步数加1 */
				g_add_pulse_count++;
				if(g_srd.dir == CW)
				{
					g_step_position++;                      /* 绝对位置加1 */
				}
				else
				{
					g_step_position--;                      /* 绝对位置减1 */
				}
				g_srd.accel_count++;
				new_step_delay = g_srd.step_delay - (((2 * g_srd.step_delay) + Pump_rest)/(4 * g_srd.accel_count + 1));  /* 计算新(下)一步脉冲周期(时间间隔) */
				Pump_rest = ((2 * g_srd.step_delay)+Pump_rest)%(4 * g_srd.accel_count + 1);                                   /* 计算余数，下次计算补上余数，减少误差 */

				/* 检查是否为最后一步 */
				if(g_srd.accel_count >= 0)                  /* 判断减速步数是否从负值加到0是的话 减速完成 */
				{
					g_srd.run_state = STOP;
				}
				break;
		}
		g_srd.step_delay = new_step_delay;              /* 为下个(新的)延时(脉冲周期)赋值 */
	}
}






