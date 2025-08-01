#include "HEAD.h"

uint16_t ADC_ConvertedValue[Sample_Num];	//ADC数据缓存区
uint32_t ADC_Value_SUM;										//采样数据平均滤波临时总和
float 	ADC_Value;												//ADC滤波后的值
float 	Battery_V=0;											//转换后电压值

uint8_t PowerEN_Flag=0;										//充电标志为，充电器插入
uint8_t PowerEN_Blink=0;									//充电时的电量动图标志计数
uint32_t ASD[20]={0};


void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef 		ADC_InitStruct;							//定义一个ADC初始化结构体
	GPIO_InitTypeDef		GPIO_InitStructur;					//定义一个GPIO初始化结构体
	
	ADC_GPIO_APBxClkCmd(ADC_GPIO_CLK_A,ENABLE);				//ADC_GPIO时钟使能
	PowerEN_GPIO_APBxClkCmd(PowerEN_GPIO_CLK,ENABLE);	//PowerENGPIO时钟使能
	ADC_APBxClock_FUN(ADC_V_CLK,ENABLE);							//ADC时钟使能
	
	//将ADC_GPO配置为模拟输入，必须为模拟输入
	GPIO_InitStructur.GPIO_Pin=ADC_V_IN1_GPIO_PIN;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(ADC_V_IN1_GPIO_PORT,&GPIO_InitStructur);
	
	//PB9为充电感应引脚，判断是否在充电
	GPIO_InitStructur.GPIO_Pin=PowerEN_GPIO_PIN;
	GPIO_InitStructur.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(PowerEN_GPIO_PORT,&GPIO_InitStructur);
	
/**************************ADC_V配置**************************/
	ADC_InitStruct.ADC_Mode=ADC_Mode_RegSimult;				 							//同步规则模式，双ADC使用。
	ADC_InitStruct.ADC_ScanConvMode=ENABLE;													//是否进入扫描模式，针对多通道而言，这里启用。
	ADC_InitStruct.ADC_ContinuousConvMode=ENABLE;										//是否连续转换，选择是。
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;	//外部触发方式，这里不选择，用软件触发。
	ADC_InitStruct.ADC_DataAlign =ADC_DataAlign_Right;							//数据的对齐方式，选择右对齐。
	ADC_InitStruct.ADC_NbrOfChannel =1;															//转换的通道数目，这里选择4，有4个通道在使用。
	
	ADC_Init(ADC_V,&ADC_InitStruct);																//寄存器初始化
	
	//配置ADC时钟，配置通道的转换顺序和采样时间
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);																//在rcc.c文件的766行的函数，72/8=9，配置为9MHz
	ADC_RegularChannelConfig(ADC_V, ADC_V_IN1_Chanel, 1, ADC_SampleTime_239Cycles5);			//ADCx；ADC通道；转换顺序，只有一个，所以为1；采样时间，55个ADC周期

	ADC_DMACmd(ADC_V, ENABLE);																			//ADC向DMA发送请求

	ADC_Cmd(ADC_V, ENABLE);																					//ADC使能
	
	//ADC_V校准
	ADC_ResetCalibration(ADC_V);																		//ADC复位校准
  while(ADC_GetResetCalibrationStatus(ADC_V));
	ADC_StartCalibration(ADC_V);																		//ADC校准
	while(ADC_GetCalibrationStatus(ADC_V));													//等待ADC校准完毕
	
	ADC_SoftwareStartConvCmd(ADC_V, ENABLE);												//由于没有采用外部触发，所以使用软件触发ADC转换。	
}

//============================================
//函数名称:void NVIC_Configuration(void)
//功能描述:中断配置
//输入:无
//输出:无
//============================================
void DMA_NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_Init(&NVIC_InitStructure);        
}

void ADC_DMA_Cofig(void)
{	
	DMA_InitTypeDef DMA_InitStruct;										//定义结构体
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);				//打开DMA1时钟
	
	DMA_DeInit(ADC_DMA_CHANEL);//复位DMA控制器
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)(&(ADC_V->DR));						//外设基地址为：ADC的数据寄存器地址，ADC1和ADC2的数据都放在同一个地址里，高16位为ADC2的，低16位为ADC1的。
	DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)ADC_ConvertedValue;					//缓存器的地址，要存放数据的地址。
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;															//外设是源	

	DMA_InitStruct.DMA_BufferSize =Sample_Num;					//缓冲区大小									
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;								//只有一个外设数据地址，不用递增
	DMA_InitStruct.DMA_PeripheralDataSize =DMA_PeripheralDataSize_HalfWord;		//数据32位，一个字符。
	DMA_InitStruct.DMA_MemoryInc =DMA_MemoryInc_Enable;												//有多个数据存储地址，要递增
	DMA_InitStruct.DMA_MemoryDataSize =DMA_MemoryDataSize_HalfWord;						//数据32位，一个字符。	
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;																//不断传输，normal为传输一次
	DMA_InitStruct.DMA_Priority=DMA_Priority_Low;															//中断优先级为高
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;																		//禁止存储器到存储器模式，因为是从外设到存储器

	DMA_ITConfig(ADC_DMA_CHANEL,DMA_IT_TC,DISABLE); //传输结束进中断
	//DMA_NVIC();//DMA中断优先级
	
	DMA_Init(ADC_DMA_CHANEL,&DMA_InitStruct);//初始化DMA
	DMA_Cmd(ADC_DMA_CHANEL,ENABLE);//使能DMA
}

void ADC_DMA_Init(void)
{
	BasicTIME7_Config();//定时器要放在ADC——DMA初始化之前，否者进中断会异常
	ADC_DMA_Cofig();
	ADCx_Mode_Config();
}

/****
4080------100%
3835------75%
3590------50%
3345------25%
3100------0%
****/

void DMA1_Channel1_IRQHandler(void)//DMA中断函数，数据处理
{
	DMA_ClearITPendingBit(DMA1_IT_TC1);//清除中断标志位
  DMA_ClearFlag(DMA1_FLAG_TC1);//清除中断标志位
}


