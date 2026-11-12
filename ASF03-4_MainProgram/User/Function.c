#include "HEAD.h"

uint8_t WorkMode=1;							//工作模式，1单次加液模式，2间隔加液模式，3持续加液模式。默认是单次加液模式
uint8_t Emergency_Stop_Flag=0;	//急停标志位

/*****************************************************************/
//校准相关
float Pump_Theoretically[4];//校准模式预计加液量，默认7000；
double Pump_Actually[4];//校准模式实际加液量。

uint8_t ScreenState=0;//当前屏幕状态，0在工作界面，1在校准界面,3其界面不动作。
/*****************************************************************/
//加液相关
uint8_t TitleFlag[4];				//提示
uint8_t SelctedChannel[4];	//选中标志位
float 	Add_Volume[4];			//加液量，单位mL
float 	Pump_OneStep[4];		//单步加液量，单位nL

uint32_t  Add_Times=0;								//加液次数，循环模式
uint32_t  Add_IntervalTime=0;					//加液间隔时间，循环模式
uint8_t   Add_IntervalTimeFlag=0;			//加液间隔时间标志位,用定时器三去定时，定时器3为10ms
uint32_t 	Add_IntervalTimeCounter=0;	//加液间隔定时器计数

float 	ALLPumpSpeed;				//蠕动泵的转速
uint8_t ALLDirFlag;					//方向标志位，1正向，0方向

uint8_t MethodHide_Flag=0;		//单标方法选择菜单栏弹出标志位
/*****************************************************************/
//校准相关
uint8_t CaSelctedChannel[4];	//选中标志位

/*****************************************************************/
float Pump_BackDraw=5;//蠕动泵回吸容量，单位mL
float Pump_FillPipe=5;//蠕动泵填充管道容量，单位mL
float Pump_ClearPipe=15;//清洗管道容量，单位mL

/*****************************************************************/
SettingParametersSTR 	SettingParameters;		//后台参数
MethodSTR 						Method;				//保存方法
uint32_t  						MethodNum;		//方法数量
uint8_t 							Method_Name_Unput_Flag=0;//方法名称录入有效标志位
uint8_t 							Method_Unput_Flag=0;		//方法管理界面方法名弹出有效标志位
uint8_t               Method_SwitchFlag=0;		//方法管理界面选中的方法的位次
/*****************************************************************
* 名    称： ASCIItoDEC()
* 功    能： ASCII码转一位十进制。
* 入口参数：*data，需要转换的值
* 出口参数： 
 *****************************************************************/
int ASCIItoDEC(uint8_t *data)
{
	if( (*data>=0x30) && (*data<=0x39) )		
		return (*data - 0x30);
	else
		return 0;
	
//	switch(*data)//屏幕控件
//	{
//		case 0x30:
//			return 0;
//		
//		case 0x31:
//			return 1;
//		
//		case 0x32:
//			return 2;
//		
//		case 0x33:
//			return 3;
//		
//		case 0x34:
//			return 4;
//		
//		case 0x35:
//			return 5;
//		
//		case 0x36:
//			return 6;
//		
//		case 0x37:
//			return 7;
//		
//		case 0x38:
//			return 8;
//		
//		case 0x39:
//			return 9;
//		
//		default:
//			break;
//	}
//	return NULL;
}

/*****************************************************************
* 名    称： ASCIItoFloat()
* 功    能： ASCII码转十进制。
* 入口参数：*Arry需要转换的字符串
* 出口参数： 
 *****************************************************************/
float ASCIItoFloat(uint8_t *Arry)
{
	float data = 0;					 /* 整数暂存 */
	float decimal_1;				 /* 小数的整数部分暂存 */
	float decimal_0;				 /* 小数的小数部分暂存 */
	int dataLength_bump = 8; /* 数据位数基数标志位 */
	int dataLength;					 /* 数据的总位数，包括小数点 */
	int point = 0;					 /* 小数的整数位数 */

	while (Arry[dataLength_bump] != 0x00)
	{
		if (Arry[dataLength_bump] == 0x2E)
			point = dataLength_bump - 8; /* 表示有小数，且poin为整数部分的位数 */
		dataLength_bump++;						 /* 判断数据位数，包含小数点 */
	}
	dataLength = dataLength_bump - 8; /* 总长度 */

	if (point != 0)
	{								 /* 是小数 */
		decimal_1 = 0; /* 整数部分 */
		for (int i = 0; i < point; i++)
			decimal_1 = decimal_1 * 10 + ASCIItoDEC(&Arry[8 + i]);

		decimal_0 = 0; /* 小数部分 */
		for (int i = (dataLength - point - 1); i > 0; i--)
			decimal_0 = decimal_0 * 0.1 + ASCIItoDEC(&Arry[8 + point + i]);
		decimal_0 = decimal_0 * 0.1;

		if (Arry[8] != 0x2D)
			return decimal_1 + decimal_0;
		else
			return -(decimal_1 + decimal_0);
	}

	else
	{ /* 是整数 */
		data = 0;
		for (int i = 0; i < dataLength; i++)
			data = data * 10 + ASCIItoDEC(&Arry[8 + i]);

		if (Arry[8] != 0x2D)
			return data;
		else
			return -data;
	}
//	float data;//整数暂存
//	float	decimal_1;//小数的整数部分暂存
//	float decimal_0;//小数的小数部分暂存
//	
//	int dataLength_bump=8;//数据位数基数标志位
//	int	dataLength;//数据的总位数，包括小数点
//	int	point=0;//小数的整数位数。
//	
//	while(Arry[dataLength_bump]!=0x00)
//	{
//		if(Arry[dataLength_bump]==0x2E)
//			point=dataLength_bump-8;//表示有小数，且poin为整数部分的位数
//		
//		dataLength_bump++;//判断数据位数，包含小数点
//	}
//	dataLength=dataLength_bump-8;//总长度
//	
//	if(point!=0)//是小数
//	{
//		switch(point)
//		{
//			case 1:
//				decimal_1=ASCIItoDEC(&Arry[8]);
//				break;
//			case 2:
//				decimal_1=ASCIItoDEC(&Arry[8])*10+ASCIItoDEC(&Arry[9]);
//				break;
//			case 3:
//				decimal_1=ASCIItoDEC(&Arry[8])*100+ASCIItoDEC(&Arry[9])*10+ASCIItoDEC(&Arry[10]);
//				break;
//			case 4:
//				decimal_1=ASCIItoDEC(&Arry[8])*1000+ASCIItoDEC(&Arry[9])*100+ASCIItoDEC(&Arry[10])*10+ASCIItoDEC(&Arry[11]);
//				break;
//			
//			default:
//				break;
//		}
//		switch(dataLength-point-1)
//		{
//			case 1:
//				decimal_0=ASCIItoDEC(&Arry[8+point+1])*0.1;
//				break;
//			case 2:
//				decimal_0=ASCIItoDEC(&Arry[8+point+1])*0.1+ASCIItoDEC(&Arry[8+point+2])*0.01;
//				break;
//			case 3:
//				decimal_0=ASCIItoDEC(&Arry[8+point+1])*0.1+ASCIItoDEC(&Arry[8+point+2])*0.01+ASCIItoDEC(&Arry[8+point+3])*0.001;
//				break;
//			case 4:
//				decimal_0=ASCIItoDEC(&Arry[8+point+1])*0.1+ASCIItoDEC(&Arry[8+point+2])*0.01+ASCIItoDEC(&Arry[8+point+3])*0.001+ASCIItoDEC(&Arry[8+point+4])*0.0001;
//				break;
//			
//			default:
//				break;
//		}
//		return decimal_1+decimal_0;		
//	}
//	
//	else//是整数
//	{
//		switch(dataLength)
//		{
//			case 1:
//				data=ASCIItoDEC(&Arry[8]);
//				break;
//			case 2:
//				data=ASCIItoDEC(&Arry[8])*10+ASCIItoDEC(&Arry[9]);
//				break;
//			case 3:
//				data=ASCIItoDEC(&Arry[8])*100+ASCIItoDEC(&Arry[9])*10+ASCIItoDEC(&Arry[10]);
//				break;
//			case 4:
//				data=ASCIItoDEC(&Arry[8])*1000+ASCIItoDEC(&Arry[9])*100+ASCIItoDEC(&Arry[10])*10+ASCIItoDEC(&Arry[11]);
//				break;
//			case 5:
//				data=ASCIItoDEC(&Arry[8])*10000+ASCIItoDEC(&Arry[9])*1000+ASCIItoDEC(&Arry[10])*100+ASCIItoDEC(&Arry[11])*10+ASCIItoDEC(&Arry[12]);
//				break;
//			case 6:
//				data=ASCIItoDEC(&Arry[8])*100000+ASCIItoDEC(&Arry[9])*10000+ASCIItoDEC(&Arry[10])*1000+ASCIItoDEC(&Arry[11])*100+ASCIItoDEC(&Arry[12])*10+ASCIItoDEC(&Arry[13]);
//				break;
//			
//			default:
//				break;
//		}
//		return data;
//	}
}

/*****************************************************************
* 名    称： FillPipe(void)
* 功    能： 填充管道功能，在加液前要对管道进行填充，避免空管造成误差
* 入口参数：
* 出口参数： 
 *****************************************************************/
void FillPipe(void)
{
	uint32_t Liq_temp=0;		//加液量临时换算
	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			Liq_temp=(Pump_FillPipe*1000.0*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
			TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
			
			if(ALLDirFlag==1)
				TMC5240_MoveTo(i+1,Liq_temp);
			else	
				TMC5240_MoveTo(i+1,-Liq_temp);
			
			if(i==0)
				Channel1_MotorStatus.Busy=1;
			if(i==1)
				Channel2_MotorStatus.Busy=1;
			if(i==2)
				Channel3_MotorStatus.Busy=1;
			if(i==3)
				Channel4_MotorStatus.Busy=1;
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));

	Beep_S();//完成提示音
	return;
}

/*****************************************************************
* 名    称： ClearPipe(void)
* 功    能： 清洗管道功能，避免污染管道，一般是填充管道的三倍，四个泵同时清洗
* 入口参数：
* 出口参数： 
 *****************************************************************/
void ClearPipe(void)
{
	uint32_t Liq_temp=0;		//加液量临时换算
	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			Liq_temp=(Pump_ClearPipe*1000.0*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
			TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
			
			if(ALLDirFlag==1)
				TMC5240_MoveTo(i+1,Liq_temp);
			else	
				TMC5240_MoveTo(i+1,-Liq_temp);
			
			if(i==0)
				Channel1_MotorStatus.Busy=1;
			if(i==1)
				Channel2_MotorStatus.Busy=1;
			if(i==2)
				Channel3_MotorStatus.Busy=1;
			if(i==3)
				Channel4_MotorStatus.Busy=1;
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));
	return;
}
/*****************************************************************
* 名    称： BackDraw(void)
* 功    能： 回吸，清除管道内的液体
* 入口参数：
* 出口参数： 
 *****************************************************************/
void BackDraw(void)
{
	uint32_t Liq_temp=0;		//加液量临时换算
	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			Liq_temp=(Pump_BackDraw*1000.0*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
			TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
			
			if(ALLDirFlag==1)
				TMC5240_MoveTo(i+1,-Liq_temp);
			else	
				TMC5240_MoveTo(i+1,Liq_temp);
			
			if(i==0)
				Channel1_MotorStatus.Busy=1;
			if(i==1)
				Channel2_MotorStatus.Busy=1;
			if(i==2)
				Channel3_MotorStatus.Busy=1;
			if(i==3)
				Channel4_MotorStatus.Busy=1;
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));
	
	Beep_S();//完成提示音
	return;
}

/*****************************************************************
* 名    称： Emergency_Stop()
* 功    能： 紧急停止函数，置位相关标志位，关闭相关使能，使电机蠕动泵不在工作
* 入口参数：
* 出口参数： 
*****************************************************************/
void Emergency_Stop(void)
{
	WorkBusy=0;
	Emergency_Stop_Flag=1;//急停打开

	TMC5240_Stop(1);//蠕动泵停止
	TMC5240_Stop(2);
	TMC5240_Stop(3);
	TMC5240_Stop(4);
	
	Channel1_MotorStatus.Busy=0;//电机不忙
	Channel2_MotorStatus.Busy=0;//电机不忙
	Channel3_MotorStatus.Busy=0;//电机不忙
	Channel4_MotorStatus.Busy=0;//电机不忙
	
	Add_IntervalTimeCounter=0;//间隔时间相关计数标志位清零
	Add_IntervalTimeFlag=1;//循环模式在急停下，标志位置1，跳出等待循环	
}

/*****************************************************************
* 名    称： Flag_Init()
* 功    能： 相关标志位的开机初始化
* 入口参数：
* 出口参数： 
 *****************************************************************/
void Flag_Init(void)
{
	//【通道提示】
	FLASH_Read(TitleFlag_ADR,(u8 *)&TitleFlag,sizeof(TitleFlag));//读取数据
	SendTitleTextValue(TitleFlag[0], 0, 40);//通道提示
	SendTitleTextValue(TitleFlag[1], 0, 41);//通道提示
	SendTitleTextValue(TitleFlag[2], 0, 42);//通道提示
	SendTitleTextValue(TitleFlag[3], 0, 43);//通道提示
	
	//【通道加液量】
	FLASH_Read(Add_Volume_ADR,(u8 *)&Add_Volume,sizeof(Add_Volume));//读取数据
	sprintf(Str,"%1.1f",Add_Volume[0]);	   //类型转换 		
	SetTextValue(0,44,Str);
	sprintf(Str,"%1.1f",Add_Volume[1]);	   //类型转换 		
	SetTextValue(0,45,Str);
	sprintf(Str,"%1.1f",Add_Volume[2]);	   //类型转换 		
	SetTextValue(0,46,Str);
	sprintf(Str,"%1.1f",Add_Volume[3]);	   //类型转换 		
	SetTextValue(0,47,Str);
	
	//【填充量】
	FLASH_Read(Pump_FillPipe_ADR,(u8 *)&Pump_FillPipe,sizeof(Pump_FillPipe));//读取数据
	Pump_BackDraw=Pump_FillPipe;			//回吸
	Pump_ClearPipe=Pump_FillPipe*3;		//清洗	
	
	//【泵旋转方向】
	FLASH_Read(ALLDirFlag_ADR,(u8 *)&ALLDirFlag,sizeof(ALLDirFlag));//读取数据
	
	//【泵旋转速度】
	FLASH_Read(ALLPumpSpeed_ADR,(u8 *)&ALLPumpSpeed,sizeof(ALLPumpSpeed));//读取数据
	
	//【单步加液量】
	FLASH_Read(Pump_OneStep_ADR,(u8 *)&Pump_OneStep,sizeof(Pump_OneStep));//读取数据
	
	//【隐藏方法选择控件】
	DisplayControl(0,23,0);		//隐藏方法选择控件
	DisplayControl(0,22,0);
	MethodHide_Flag=0;
	
	//【方法名称显示归零】
	SetTextValue(0,14,"*");//显示方法名称
	
	//【隐藏黄色充电图标】
	DisplayControl(0,16,0);//隐藏图标，闪电黄标
	
	//通道选中提示
	for(int i=0; i<4;i++)
	{
		SelctedChannel[i]=1;
		DisplayControl(0,24+i,1);//选中
		SwitchSet(0,30+i);	//按下回吸按钮	
	}
	
	SetTextValue(0,17,"单次加液");
	
	DisplayControl(0, 18,0);//隐藏之画面1	
	
	Emergency_Stop_Flag=0;//紧急标志位置0
	
}

/*****************************************************************
* 名    称： Clear_TFT_USART_RX_Buf()
* 功    能： 清空接收BUFF
* 入口参数：
* 出口参数： 
 *****************************************************************/
void Clear_TFT_USART_RX_Buf(void)
{
	for(int i=0;i<RX_BUFF_LENGTH;i++)
	{
		TFT_USART_RX_BUF[i]=0;
	}
}

/*****************************************************************
* 名    称： WorkMode_Single()
* 功    能： 单次加液模式
* 入口参数：
* 出口参数： 
 *****************************************************************/
void WorkMode_Single(void)
{
	uint32_t Liq_temp=0;		//加液量临时换算
	Systick_Delay_ms(50);

	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			Liq_temp=(Add_Volume[i]*1000.0*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
			TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
			
			if(ALLDirFlag==1)
				TMC5240_MoveTo(i+1,Liq_temp);
			else	
				TMC5240_MoveTo(i+1,-Liq_temp);
			
			if(i==0)
				Channel1_MotorStatus.Busy=1;
			if(i==1)
				Channel2_MotorStatus.Busy=1;
			if(i==2)
				Channel3_MotorStatus.Busy=1;
			if(i==3)
				Channel4_MotorStatus.Busy=1;
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));

	Systick_Delay_ms(200);
	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			if (SettingParameters.PipeCallBackVolume != 0)
			{
				Liq_temp=(SettingParameters.PipeCallBackVolume*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
				TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm

				if(ALLDirFlag==1)
					TMC5240_MoveTo(i+1,-Liq_temp);
				else	
					TMC5240_MoveTo(i+1,+Liq_temp);

				if(i==0)
					Channel1_MotorStatus.Busy=1;
				if(i==1)
					Channel2_MotorStatus.Busy=1;
				if(i==2)
					Channel3_MotorStatus.Busy=1;
				if(i==3)
					Channel4_MotorStatus.Busy=1;
			}
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));
	
	Beep_S();//完成提示音
	Clear_TFT_USART_RX_Buf();//清空接收BUFF，防止在运行的过程中误触进入其他程序。
	return;
}
/*****************************************************************
* 名    称： WorkMode_Interval()
* 功    能： 间隔加液模式，间隔有时间，有加液次数
* 入口参数：
* 出口参数： 
 *****************************************************************/
void WorkMode_Interval(void)
{
	uint32_t Liq_temp=0;		//加液量临时换算
	
	while(Add_Times!=0)
	{
		if(Emergency_Stop_Flag==1)//急停打开
			break;
		
			for(int i=0;i<4;i++)
		{
			if(SelctedChannel[i]==1)
			{//通道选中
				Liq_temp=(Add_Volume[i]*1000.0*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；加液1000L
				TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
				
				if(ALLDirFlag==1)
					TMC5240_MoveTo(i+1,Liq_temp);
				else	
					TMC5240_MoveTo(i+1,-Liq_temp);
				
				if(i==0)
					Channel1_MotorStatus.Busy=1;
				if(i==1)
					Channel2_MotorStatus.Busy=1;
				if(i==2)
					Channel3_MotorStatus.Busy=1;
				if(i==3)
					Channel4_MotorStatus.Busy=1;
			}
		}
		//等待加液完成
		while(	(Channel1_MotorStatus.Busy==1)
					||(Channel2_MotorStatus.Busy==1)
					||(Channel3_MotorStatus.Busy==1)
					||(Channel4_MotorStatus.Busy==1));
		
		Add_Times--;
		sprintf(Str,"%d",Add_Times);	 //类型转换
		SetTextValue(2,3,Str);	

		if(Emergency_Stop_Flag==0)//没有急停时更新相应标志位，有急停时不更新，避免进入死循环
		{
			Add_IntervalTimeCounter=0;//间隔时间相关计数标志位清零，等待间隔时间
			Add_IntervalTimeFlag=0;
		}
		while(Add_IntervalTimeFlag==0);//等待间隔时间到
	}

	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			if (SettingParameters.PipeCallBackVolume != 0)
			{
				Liq_temp=(SettingParameters.PipeCallBackVolume*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
				TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm

				if(ALLDirFlag==1)
					TMC5240_MoveTo(i+1,-Liq_temp);
				else	
					TMC5240_MoveTo(i+1,+Liq_temp);

				if(i==0)
					Channel1_MotorStatus.Busy=1;
				if(i==1)
					Channel2_MotorStatus.Busy=1;
				if(i==2)
					Channel3_MotorStatus.Busy=1;
				if(i==3)
					Channel4_MotorStatus.Busy=1;
			}
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));

	Add_Times=0;
	Beep_S();//完成提示音
	sprintf(Str,"%d",(uint32_t)Add_Times);	 //类型转换
	SetTextValue(2,3,Str);	
	Clear_TFT_USART_RX_Buf();//清空接收BUFF，防止在运行的过程中误触进入其他程序。
	return;
}
/*****************************************************************
* 名    称： WorkMode_Continued()
* 功    能： 持续加液，一直不断的加液
* 入口参数：
* 出口参数： 
 *****************************************************************/
void WorkMode_Continued(void)
{
	uint32_t Liq_temp=0;		//加液量临时换算
	Systick_Delay_ms(50);

	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
			
			if(ALLDirFlag==1)				
				TMC5240_MoveGo(i+1,1);
			else	
				TMC5240_MoveGo(i+1,0);
			
			if(i==0)
				Channel1_MotorStatus.Busy=1;
			if(i==1)
				Channel2_MotorStatus.Busy=1;
			if(i==2)
				Channel3_MotorStatus.Busy=1;
			if(i==3)
				Channel4_MotorStatus.Busy=1;
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));

	Systick_Delay_ms(700);
	for(int i=0;i<4;i++)
	{
		if(SelctedChannel[i]==1)
		{//通道选中
			if (SettingParameters.PipeCallBackVolume != 0)
			{
				Liq_temp=(SettingParameters.PipeCallBackVolume*1000.0*256/Pump_OneStep[i]);//计步，以256微步计算；
				TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm

				if(ALLDirFlag==1)
					TMC5240_MoveTo(i+1,-Liq_temp);
				else	
					TMC5240_MoveTo(i+1,+Liq_temp);

				if(i==0)
					Channel1_MotorStatus.Busy=1;
				if(i==1)
					Channel2_MotorStatus.Busy=1;
				if(i==2)
					Channel3_MotorStatus.Busy=1;
				if(i==3)
					Channel4_MotorStatus.Busy=1;
			}
		}
	}
	//等待加液完成
	while(	(Channel1_MotorStatus.Busy==1)
				||(Channel2_MotorStatus.Busy==1)
				||(Channel3_MotorStatus.Busy==1)
				||(Channel4_MotorStatus.Busy==1));
	
	Beep_S();//完成提示音
	Clear_TFT_USART_RX_Buf();//清空接收BUFF，防止在运行的过程中误触进入其他程序。
	return;
}


/**********************************************************************************************************
*	函 数 名: ReadMethod_Show(uint8_t Method_Num,uint8_t Single_AorB)
*	功能说明: 读取保存的方法并显示出来
*	形    参：Single_Method_Num：方法数量，第几个方法
						Single_AorB：保存标志位，用来区分单标序列A，序列B，混标。
*	返 回 值: 
**********************************************************************************************************/
void ReadMethod_Show(void)
{
	//【通道提示】
	SendTitleTextValue(Method.TitleFlag[0], 0, 40);//通道提示
	SendTitleTextValue(Method.TitleFlag[1], 0, 41);//通道提示
	SendTitleTextValue(Method.TitleFlag[2], 0, 42);//通道提示
	SendTitleTextValue(Method.TitleFlag[3], 0, 43);//通道提示
	
	//【通道加液量】
	sprintf(Str,"%1.1f",Method.Add_Volume[0]);	   //类型转换 		
	SetTextValue(0,44,Str);
	sprintf(Str,"%1.1f",Method.Add_Volume[1]);	   //类型转换 		
	SetTextValue(0,45,Str);
	sprintf(Str,"%1.1f",Method.Add_Volume[2]);	   //类型转换 		
	SetTextValue(0,46,Str);
	sprintf(Str,"%1.1f",Method.Add_Volume[3]);	   //类型转换 		
	SetTextValue(0,47,Str);
	
	//【方法名称显示】
	SetTextValue(0,14,Method.Name);//显示方法名称
	
	//通道选中提示
	for(int i=0; i<4;i++)
	{
		if(Method.SelctedChannel[i]==1)
		{
			DisplayControl(0,24+i,1);//选中
			SwitchSet(0,30+i);	//按下回吸按钮	
		}
		else
		{
			DisplayControl(0,24+i,0);//不选中
			SetTextValue(0,44+i,"0");
			SwitchReSet(0,30+i);	//按下回吸按钮	
		}
	}

	//【模式】
	WorkMode=Method.WorkMode;
	switch(WorkMode)
	{
		case 1:	//1单次加液模式
			SetTextValue(0,17,"单次加液");
			DisplayControl(0, 18,0);//隐藏之画面1		
			for(int i=0;i<4;i++)
			{
				if(Method.SelctedChannel[i]==1)
				{
					sprintf(Str,"%1.1f",Add_Volume[i]);	 //类型转换
					SetTextValue(0,44+i,Str);
				}					
			}			
			break;
		case 2: //2间隔加液模式
			SetTextValue(0,17,"循环加液");
			DisplayControl(0, 18,1);//显示子画面2				

			Add_Times=Method.Add_Times;
			Add_IntervalTime=Method.Add_IntervalTime;
			sprintf(Str,"%d",Add_Times);	 //类型转换
			SetTextValue(2,3,Str);	
			sprintf(Str,"%d",Add_IntervalTime);	 //类型转换
			SetTextValue(2,4,Str);	
			for(int i=0;i<4;i++)
			{
				if(Method.SelctedChannel[i]==1)
				{
					sprintf(Str,"%1.1f",Add_Volume[i]);	 //类型转换
					SetTextValue(0,44+i,Str);
				}						
			}			

			break;
		case 3: //3持续加液模式
			SetTextValue(0,17,"持续加液");
			DisplayControl(0, 18,0);//隐藏之画面1		
			for(int i=0;i<4;i++)
				SetTextValue(0,44+i,"-");	
	
			break;
		
		default:
			break;
	}						
	return;
}

/**********************************************************************************************************
*	函 数 名: ReadMethodManage_Show(uint8_t Method_Num,uint8_t Single_AorB)
*	功能说明: 读取保存的方法并显示出来，方法管理姐买你
*	形    参：Single_Method_Num：方法数量，第几个方法
						Single_AorB：保存标志位，用来区分单标序列A，序列B，混标。
*	返 回 值: 
**********************************************************************************************************/
void ReadMethodManage_Show(uint8_t Method_Num)
{
	FLASH_Read(Method_ADR+sizeof(Method)*(Method_Num-1),(u8 *)&Method,sizeof(Method));//读出数据	
	
	for(int i=0;i<4;i++)
	{
		SendTitleTextValue(Method.TitleFlag[i], 9, 6+i);//通道提示
		sprintf(Str,"%1.1f",Method.Add_Volume[i]);	   //类型转换 		
		SetTextValue(9,10+i,Str);//【通道加液量】
		if(Method.SelctedChannel[i]==1)
			DisplayControl(9,19+i,1);//通道选中提示	
		else
		{
			DisplayControl(9,19+i,0);//通道不选中提示	
			SetTextValue(9,10+i,"0");
		}
	}
	
	if(Method.WorkMode==2)
	{
		sprintf(Str,"%d",Method.Add_Times);	 //类型转换
		SetTextValue(9,25,Str);	
		sprintf(Str,"%d",Method.Add_IntervalTime);	//类型转换
		SetTextValue(9,26,Str);
	}
	else
	{
		SetTextValue(9,25,"*");	
		SetTextValue(9,26,"*");
	}
	
	if(Method.WorkMode==3)
	{
		for(int i=0;i<4;i++)
				SetTextValue(9,10+i,"-");	
	}
	return;
}


