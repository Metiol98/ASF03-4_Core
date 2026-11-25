#include "Function.h"

static uint8_t str[50]={0};
volatile uint8_t WorkMode = SINGLE_MODE;	//工作模式，1单次加液模式，2间隔加液模式，3持续加液模式
MCBConfigSTR	g_MCBConfig;
NowStatusSTR	g_NowStatus;
IntervalSTR 	g_Interval;
CaliSTR 			g_Cali;
BatterySTR    g_Battery;

SaveMethodSTR 	 SaveMethod[MAX_METHOD_COUNT];

PendingCMDSTR		g_TFTPendingCMD; 

/**
  * @brief  系统初始化
  * @param  None.
  * @retval None.
  */
void System_Init(void)
{
	RingBuff_Init(&g_DisPlayRingBuff);		/* 来自屏幕指令的数据环 */
	
	Method_Init();
	buzzer_init();
	LCD_init();
	RFID_init();
	BAT_Init();
	TIM_Init();
	TMC5240_init();
//	W25Qx_Init();
//	
//	W25Qx_Test();
}

/**
  * @brief  出厂设置，所有参数恢复到默认值
  * @param  None.
  * @retval None. 
  */
void FactorySettings_Init(void)
{

}


/**
  * @brief  开机初始化
  * @param  None.
  * @retval None. 
  */
void Flag_Init(void)
{
	uint8_t pump_due_mask;

	FLASH_Param_ReadWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
	FLASH_Param_ReadWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));

	//【通道提示】
	TFTDisplay_SetTextValue(0,40,g_MCBConfig.TitleName[0]);
	TFTDisplay_SetTextValue(0,41,g_MCBConfig.TitleName[1]);
	TFTDisplay_SetTextValue(0,42,g_MCBConfig.TitleName[2]);
	TFTDisplay_SetTextValue(0,43,g_MCBConfig.TitleName[3]);
	
	//【通道加液量】
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[0]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,44,g_str);
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[1]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,45,g_str);
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[2]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,46,g_str);
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[3]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,47,g_str);

	//【弹起按钮】
	TFTDisplay_DisplayControl(0,70,USER_DISPLAY);
	TFTDisplay_DisplayControl(0,71,USER_DISPLAY);
	TFTDisplay_DisplayControl(0,72,USER_DISPLAY);
	TFTDisplay_DisplayControl(0,73,USER_DISPLAY);
	
	//【泵旋转方向】
	//【泵旋转速度】
	
	//【电机电流设置】
	TMC5240_WriteReadRegister(1,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
	TMC5240_WriteReadRegister(2,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
	TMC5240_WriteReadRegister(3,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
	TMC5240_WriteReadRegister(4,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);

	//【隐藏方法选择控件】
	TFTDisplay_DisplayControl(0,23,0);		//隐藏方法选择控件
	TFTDisplay_DisplayControl(0,22,0);

	//【方法名称显示归零】
	TFTDisplay_SetTextValue(0,14,"*");//显示方法名称

	//【隐藏黄色充电图标】
	TFTDisplay_DisplayControl(0,16,0);//隐藏图标，闪电黄标

	//【通道选中提示】
	for (int8_t i = 0; i < 4; i++)
	{
		g_NowStatus.SelectedChannel[i] = USER_TRUE;
		TFTDisplay_DisplayControl(0,24+i,1);//选中
		TFTDisplay_DisplayControl(0,70+i,1);//按下按钮
	}
	
	//按已保存的泵管状态初始化主界面提示图标
	pump_due_mask = PumpTube_GetDueMask();
	for (int8_t i = 0; i < 4; i++)
	{
		TFTDisplay_DisplayControl(0,96+i,
		                          (pump_due_mask & (uint8_t)(1U << i)) != 0U);
	}

	TFTDisplay_SetTextValue(0,17,"单次加液");

	TFTDisplay_DisplayControl(0, 18,0);//隐藏之画面1
	
	g_NowStatus.EmergencyStop = USER_FREE;
}

/**
  * @brief  单次加液模式
  * @param  None.
  * @retval None.
  */
void WorkMode_Single(void)
{
	g_NowStatus.WorkingBusy = USER_BUSY;
	
	for(int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
			PumpMotion(i+1,CW,g_MCBConfig.AddVolume[i],NoWait); //只置标志位，不等待，在后面等待
		
		if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));

	SysTickDelay_ms(400);
	for (int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
		{
			if(g_MCBConfig.PipeCallBackVolume[i] != 0)
				PumpMotion(i+1,CCW,(float)g_MCBConfig.PipeCallBackVolume[i]/1000,NoWait); //只置标志位，不等待，在后面等待

			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));

	Beep_S();//完成提示音
	g_NowStatus.WorkingBusy = USER_FREE;
}

/**
  * @brief  间隔加液模式，间隔有加液量，时间和加液次数
  * @param  None.
  * @retval None.
  */
void WorkMode_Interval(void)
{
	uint32_t Add_time = g_Interval.AddNumber;
	
	g_NowStatus.WorkingBusy = USER_BUSY;
	for(int i=0;i<g_Interval.AddNumber;i++)
	{
		/* 急停打开退出循环 */
		if(g_NowStatus.EmergencyStop == USER_TRUE)	
		break;
		
		for(int8_t i = 0; i < 4; i++)
		{
			if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
				PumpMotion(i+1,CW,g_MCBConfig.AddVolume[i],NoWait); //只置标志位，不等待，在后面等待

			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
		//等待加液完成
		while((Channel1_MotorStatus.Busy == USER_BUSY)
				||(Channel2_MotorStatus.Busy == USER_BUSY)
				||(Channel3_MotorStatus.Busy == USER_BUSY)
				||(Channel4_MotorStatus.Busy == USER_BUSY));
		
		Add_time--;
		sprintf((char*)str,"%d",Add_time);	/* 类型转换 */
		TFTDisplay_SetTextValue(2,3,(char*)str);
		
		if(g_NowStatus.EmergencyStop == USER_FALSE)
		{/* 没有急停时更新相应标志位，又急停不更新，避免进入死循环 */
			g_Interval.InterTimeCNT = 0;	/* 间隔时间相关计数标志位清零，在定时器中刷新 */
			g_Interval.InterTimeOver = USER_FALSE;
		}
		while(g_Interval.InterTimeOver == USER_FALSE)	//等待间隔时间到
		{
			__NOP();
		}
	}

	for (int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
		{
			if(g_MCBConfig.PipeCallBackVolume[i] != 0)
				PumpMotion(i+1,CCW,(float)g_MCBConfig.PipeCallBackVolume[i]/1000,NoWait); //只置标志位，不等待，在后面等待

			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));
	
	g_Interval.AddNumber = 0;												/* 加液间隔次数清零 */
	Beep_S();	/* 完成提示音 */
	sprintf((char*)str,"%d",g_Interval.AddNumber);	/* 类型转换 */
	TFTDisplay_SetTextValue(2,3,(char*)str);
	g_NowStatus.WorkingBusy = USER_FREE;
}

/**
  * @brief  持续加液，一直不停地加
  * @param  None.
  * @retval None.
  */
void WorkMode_Continued(void)
{
	g_NowStatus.WorkingBusy = USER_BUSY;
	
	SysTickDelay_ms(100);
	for(int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
			PumpRotate(i+1,CW,NoWait); //只置标志位，不等待，在后面等待

		if(i==0)
			Channel1_MotorStatus.Busy=USER_BUSY;
		if(i==1)
			Channel2_MotorStatus.Busy=USER_BUSY;
		if(i==2)
			Channel3_MotorStatus.Busy=USER_BUSY;
		if(i==3)
			Channel4_MotorStatus.Busy=USER_BUSY;
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));
	
	SysTickDelay_ms(700);

	/* 若是0代表回调不开启 */
	for (int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
		{
			if(g_MCBConfig.PipeCallBackVolume[i] != 0)
				PumpMotion(i+1,CCW,(float)g_MCBConfig.PipeCallBackVolume[i]/1000,NoWait); //只置标志位，不等待，在后面等待

			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));
	
	Beep_S();/* 完成提示音 */
	g_NowStatus.WorkingBusy = USER_FREE;
}

/**
  * @brief  管道清理，清洗量是填充量的3倍
  * @param  None.
  * @retval None.
  */
void ClearPipe(void)
{
	g_NowStatus.WorkingBusy = USER_BUSY;
	
	for(int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
		{
			PumpMotion(i+1,CW,g_MCBConfig.PipeClearVolume[i],NoWait); //只置标志位，不等待，在后面等待
			
			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));

	Beep_S();/* 完成提示音 */
}

/**
  * @brief  管道回吸，回吸量是填充量的3倍
  * @param  None.
  * @retval None.
  */
void BackSuck(void)
{
	g_NowStatus.WorkingBusy = 3;
	
	for(int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
		{
			PumpMotion(i+1,CCW,g_MCBConfig.PipeBackDrawVolume[i],NoWait); //只置标志位，不等待，在后面等待
			
			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));
	
	g_NowStatus.WorkingBusy = USER_FREE;
}

/**
  * @brief  管道填充
  * @param  None.
  * @retval None.
  */
void FillPipe(void)
{
	g_NowStatus.WorkingBusy = 2;
	
	for(int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)//通道选中
		{
			PumpMotion(i+1,CW,g_MCBConfig.PipeFillVolume[i],NoWait); //只置标志位，不等待，在后面等待
			
			if(i==0)
				Channel1_MotorStatus.Busy=USER_BUSY;
			if(i==1)
				Channel2_MotorStatus.Busy=USER_BUSY;
			if(i==2)
				Channel3_MotorStatus.Busy=USER_BUSY;
			if(i==3)
				Channel4_MotorStatus.Busy=USER_BUSY;
		}
	}
	//等待加液完成
	while((Channel1_MotorStatus.Busy == USER_BUSY)
			||(Channel2_MotorStatus.Busy == USER_BUSY)
			||(Channel3_MotorStatus.Busy == USER_BUSY)
			||(Channel4_MotorStatus.Busy == USER_BUSY));

	g_NowStatus.WorkingBusy = USER_FREE;
}

/**
  * @brief  方法选择刷新界面显示
  * @param  None.
  * @retval None.
  */
void Method_RefreshDisplay(void)
{
	//【通道提示】
	TFTDisplay_SetTextValue(0,40,g_MCBConfig.TitleName[0]);
	TFTDisplay_SetTextValue(0,41,g_MCBConfig.TitleName[1]);
	TFTDisplay_SetTextValue(0,42,g_MCBConfig.TitleName[2]);
	TFTDisplay_SetTextValue(0,43,g_MCBConfig.TitleName[3]);

	//【通道加液量】
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[0]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,44,g_str);
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[1]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,45,g_str);
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[2]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,46,g_str);
	sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[3]);	   //类型转换 		
	TFTDisplay_SetTextValue(0,47,g_str);

	//通道选中提示
	for (int8_t i = 0; i < 4; i++)
	{
		if(g_NowStatus.SelectedChannel[i] == USER_TRUE)
		{
			TFTDisplay_DisplayControl(0,24+i,1);//选中
			TFTDisplay_DisplayControl(0,70+i,USER_DISPLAY);//按下按钮
		}
		else
		{
			TFTDisplay_DisplayControl(0,24+i,0);//未选中
			TFTDisplay_DisplayControl(0,70+i,USER_HIDE);
		}
	}
	
	switch(WorkMode)
	{
		case 1:	//1单次加液模式
			TFTDisplay_SetTextValue(0,17,"单次加液");
			TFTDisplay_DisplayControl(0, 18,0);//隐藏之画面1		
			for(int i=0;i<4;i++)
			{
				if(g_NowStatus.SelectedChannel[i] == USER_TRUE)
				{
					sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[i]);	 //类型转换
					TFTDisplay_SetTextValue(0,44+i,g_str);
				}
				TFTDisplay_DisplayControl(0,4+i,USER_DISPLAY);//显示mL单位
			}			
			break;

		case 2: //2间隔加液模式
			TFTDisplay_SetTextValue(0,17,"循环加液");
			TFTDisplay_DisplayControl(0, 18,1);//显示子画面2			

			sprintf(g_str,"%d",g_Interval.AddNumber);	 //类型转换
			TFTDisplay_SetTextValue(2,3,g_str);	
			sprintf(g_str,"%1.1f",g_Interval.IntervalTime);	 //类型转换
			TFTDisplay_SetTextValue(2,4,g_str);	
			for(int i=0;i<4;i++)
			{
				if(g_NowStatus.SelectedChannel[i]==1)
				{
					sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[i]);	 //类型转换
					TFTDisplay_SetTextValue(0,44+i,g_str);
				}
				TFTDisplay_DisplayControl(0,4+i,USER_DISPLAY);//显示mL单位
			}			
			break;

		case 3: //3持续加液模式
			TFTDisplay_SetTextValue(0,17,"持续加液");
			TFTDisplay_DisplayControl(0, 18,0);//隐藏之画面1		
			for(int i=0;i<4;i++)
			{
				TFTDisplay_SetTextValue(0,44+i,"-");	
				TFTDisplay_DisplayControl(0,4+i,USER_HIDE);//显示mL单位
			}
			break;
		
		default:
			break;
	}
}

/**
  * @brief  紧急停止，标志位置位，关闭相关使能，使电机蠕动泵不再工作
  * @param  None.
  * @retval None.
  */
void Emergency_Stop(void)
{
	g_NowStatus.WorkingBusy = USER_FREE;
	g_NowStatus.EmergencyStop = USER_TRUE;//急停打开

	PumpStop(1);//蠕动泵停止
	PumpStop(2);
	PumpStop(3);
	PumpStop(4);

	Channel1_MotorStatus.Busy = USER_FREE;//电机不忙
	Channel2_MotorStatus.Busy = USER_FREE;//电机不忙
	Channel3_MotorStatus.Busy = USER_FREE;//电机不忙
	Channel4_MotorStatus.Busy = USER_FREE;//电机不忙

	g_Interval.InterTimeCNT = 0;//间隔时间相关计数标志位清零
	g_Interval.InterTimeOver = USER_TRUE;//循环模式在急停下，标志位置1，跳出等待循环
}

/**
  * @brief  急停判断程序
  * @param  None.
  * @retval None.
  */
void EmergencyStopJudge(uint8_t *Arry)
{
	uint32_t ScreenID = Arry[3]*256 + Arry[4];
	uint32_t ButtonID = Arry[5]*256 + Arry[6];
	
	/* 主界面开始按钮停止 */
	if((g_NowStatus.WorkingBusy == 1) && (ScreenID == 0) && (ButtonID == 13))
		Emergency_Stop();
	
	/* 工作界面填充按钮停止 */
	if((g_NowStatus.WorkingBusy == 2) && (ScreenID == 0) && (ButtonID == 2))
		Emergency_Stop();
		
	/* 工作界面回吸按钮停止 */
	if((g_NowStatus.WorkingBusy == 3) && (ScreenID == 0) && (ButtonID == 3))
		Emergency_Stop();
	
	/* 校准界面填充按钮停止 */
	if((g_NowStatus.WorkingBusy == 4) && (ScreenID == 1) && (ButtonID == 29))
		Emergency_Stop();

	/* 校准界面加液按钮停止 */
	if((g_NowStatus.WorkingBusy == 5) && (ScreenID == 1) && (ButtonID == 30))
		Emergency_Stop();

	/* 校准界面清洗按钮停止 */
	if((g_NowStatus.WorkingBusy == 6) && (ScreenID == 1) && (ButtonID == 31))
		Emergency_Stop();
}












