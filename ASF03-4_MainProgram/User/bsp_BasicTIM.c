#include "HEAD.h"

/*********************************************************
函数名称：BasicTIME7_NVIC(void)
描    述：定时器优先级初始化。
形    参：
*********************************************************/
void BasicTIME7_NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructur;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);									//分组	
	NVIC_InitStructur.NVIC_IRQChannel=Basic_TIME7_IRQ;							//通道
	NVIC_InitStructur.NVIC_IRQChannelPreemptionPriority=1;					//抢占优先级，主优先级,查看分组表；
	NVIC_InitStructur.NVIC_IRQChannelSubPriority =2;								//子优先级；
	NVIC_InitStructur.NVIC_IRQChannelCmd =ENABLE;										//开使能；
	NVIC_Init(&NVIC_InitStructur);																	//NVIC初始化；
}

/*********************************************************
函数名称：BasicTIME7_Config(void)
描    述：定时器7初始化函数。
形    参：
*********************************************************/
void BasicTIME7_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBasicStructure;//定义一个初始化结构体

	Basic_TIME7_APBxClock_FUN(Basic_TIME7_CLK , ENABLE);//时钟使能
	
	TIM_TimeBasicStructure.TIM_Period=Basic_TIME7_Period;			//自动重装载寄存器周的值（计数值）
	TIM_TimeBasicStructure.TIM_Prescaler=Basic_TIME7_Prescaler;		//时钟预分频数为71，则驱动计数器的时钟为CK_CNT=CK_INT/(71+1)=1M
//	TIM_TimeBasicStructure.TIM_ClockDivision=TIM_CKD_DIV1;//基本定时器不用
//	TIM_TimeBasicStructure.TIM_CounterMode=TIM_CounterMode_Up;//基本定时器不用
//	TIM_TimeBasicStructure.TIM_RepetitionCounter=0;//基本定时器不用
	
	TIM_TimeBaseInit(Basic_TIME7, &TIM_TimeBasicStructure);//初始化定时器
	
	TIM_ClearFlag(Basic_TIME7, TIM_FLAG_Update);//清除计数器中断标志位
	
	TIM_ITConfig(Basic_TIME7, TIM_IT_Update, ENABLE);//开启计数器中断
	
	TIM_Cmd(Basic_TIME7, ENABLE);//使能计数器
}

void BasicTIME7_Init(void)
{
	BasicTIME7_NVIC();		//定时器优先级初始化
	BasicTIME7_Config();	//定时器初始化
}


/*********************************************************
函数名称：Basic_TIME7_IRQHandler(void)
描    述：定时器7中断函数。50ms
形    参：
*********************************************************/
uint32_t TIM7_Counter=0;				//定时器计数
uint32_t TIM7_BatteryCounter;		//定时器计数，电量


void Basic_TIME7_IRQHandler(void)
{
	if(TIM_GetITStatus(Basic_TIME7, TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(Basic_TIME7, TIM_IT_Update);//清除标志位
		
		TIM7_Counter++;
		TIM7_BatteryCounter++;
		if(TIM7_BatteryCounter==400)
			TIM7_BatteryCounter=0;
		
		//【循环模式，间隔计数】
		if((WorkMode==2)&&(WorkBusy==1))
			Add_IntervalTimeCounter++;
		
		if(Add_IntervalTimeCounter>=Add_IntervalTime*20)
		{
			Add_IntervalTimeCounter=0;
			Add_IntervalTimeFlag=1;//循环模式间隔时间到时间到！！！
		}
		
		//【按键粘连】
		SRX885Counter++;//计数器累加，防止按键粘连重复进入程序
		if(SRX885Counter>10)
		{
			SRX885Counter=10;//防止超限制
			SRX885_Finish=1;//连续按键之间又足够时间间隔，没有粘连（按住不放）
		}
	
		
		if(TIM7_Counter>4)
		{//每200ms采集轮询一次数据
			TIM7_Counter=0;
			
			if(Channel1_MotorStatus.Busy==1)//电机忙，要实时查询电机状态
				TMC5240_QueryMotorStatus(1);
			
			if(Channel2_MotorStatus.Busy==1)//电机忙，要实时查询电机状态
				TMC5240_QueryMotorStatus(2);
			
			if(Channel3_MotorStatus.Busy==1)//电机忙，要实时查询电机状态
				TMC5240_QueryMotorStatus(3);
			
			if(Channel4_MotorStatus.Busy==1)//电机忙，要实时查询电机状态
				TMC5240_QueryMotorStatus(4);		
		}
		
		
		//【电量监测】
		if((TIM7_BatteryCounter%20)==0)
		{	//1S检测一次电量
			
			ADC_Value_SUM=0;
			for(int j=0; j<Sample_Num; j++)
			{
				ADC_Value_SUM +=(ADC_ConvertedValue[j] & 0x0FFF);
			}
			ADC_Value=ADC_Value_SUM/Sample_Num;
			Battery_V=(float)((((ADC_Value*3.3)/4095)*11.5)/1.5);
			
			//【充电器检测，是否插上充电器】
			if(GPIO_ReadInputDataBit(PowerEN_GPIO_PORT, PowerEN_GPIO_PIN)!=PowerEN_Flag)//充电出图标显示
			{
				PowerEN_Flag=GPIO_ReadInputDataBit(PowerEN_GPIO_PORT, PowerEN_GPIO_PIN);
				if(PowerEN_Flag==1)
				{
					DisplayControl(0,16,1);//显示图标，闪电黄标
				}
				if(PowerEN_Flag==0)
				{
					DisplayControl(0,16,0);//隐藏图标，闪电黄标
				}
			}
			
			//【充电动图】
			if(PowerEN_Flag==1)
			{
				DisplayControl(0,16,1);//显示充电图标，闪电黄标
				
				PowerEN_Blink++;
				if(PowerEN_Blink==4)
				{
					if(ADC_Value<3345)
						PowerEN_Blink=0;
					if((3345<=ADC_Value)&&(ADC_Value<3590))
						PowerEN_Blink=1;
					if((3590<=ADC_Value)&&(ADC_Value<3835))
						PowerEN_Blink=2;
					if((3835<=ADC_Value))
						PowerEN_Blink=3;
				}
				switch(PowerEN_Blink)
				{
					case 0:
						DisplayControl(0,53,0);
						DisplayControl(0,52,0);
						DisplayControl(0,51,0);
						DisplayControl(0,50,1);
						break;
					case 1:
						DisplayControl(0,53,0);
						DisplayControl(0,52,0);
						DisplayControl(0,51,1);
						DisplayControl(0,50,0);
						break;
					case 2:
						DisplayControl(0,53,0);
						DisplayControl(0,52,1);
						DisplayControl(0,51,0);
						DisplayControl(0,50,0);
						break;
					case 3:
						DisplayControl(0,53,1);
						DisplayControl(0,52,0);
						DisplayControl(0,51,0);
						DisplayControl(0,50,0);
						break;
					
					default:
						break;	
				}
			}	
		}

		if((TIM7_BatteryCounter%40)==0)
		{	//1S检测一次电量			
			if(PowerEN_Flag==0)
			{//没有插上充电器
				if(ADC_Value<3345)
				{
					DisplayControl(0,53,0);
					DisplayControl(0,52,0);
					DisplayControl(0,51,0);
					DisplayControl(0,50,1);
				}
				if((3345<=ADC_Value)&&(ADC_Value<3590))
				{
					DisplayControl(0,53,0);
					DisplayControl(0,52,0);
					DisplayControl(0,51,1);
					DisplayControl(0,50,0);
				}
				if((3590<=ADC_Value)&&(ADC_Value<3835))
				{
					DisplayControl(0,53,0);
					DisplayControl(0,52,1);
					DisplayControl(0,51,0);
					DisplayControl(0,50,0);
				}
				if((3835<=ADC_Value))
				{
					DisplayControl(0,53,1);
					DisplayControl(0,52,0);
					DisplayControl(0,51,0);
					DisplayControl(0,50,0);
				}		
			}
		}
	}
	TIM_ClearITPendingBit(Basic_TIME7, TIM_IT_Update);//清除标志位
}

