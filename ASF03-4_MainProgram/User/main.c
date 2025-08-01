/************************************
4通道手持加液器
MD V2.0


************************************/
#include "HEAD.h"


char Str[30]={0};				//临时存储变量

uint8_t Test;
int main(void)
{
	RCC_Configuration();		//时钟初始化
	BEEP_Init();						//蜂鸣器初始化
	TFT_Display_Init();			//串口屏幕初始化
	ADC_DMA_Init();					//电量监测ADC初始化
	SRX885_UART_Init();			//无线模块初始化
	Motor_SPI_Init();				//电机SPI初始化
	BasicTIME7_Init();	//基本定时器初始化
	
	Systick_Delay_ms(500);	//等待外部电器件上电
	FLASH_Read(SettingParameters_ADR,(u8 *)&SettingParameters,sizeof(SettingParameters));//读取数据

	TMC5240_Init(1);				//通道初始化
	TMC5240_Init(2);
	TMC5240_Init(3);
	TMC5240_Init(4);
	
	Systick_Delay_ms(500);	//等待外部电器件上电

	Motor_SPI_EN1_ON;				//通道使能打开
	Motor_SPI_EN2_ON;
	Motor_SPI_EN3_ON;
	Motor_SPI_EN4_ON;
	
	
//	Test=sizeof(Method);
	
	Systick_Delay_ms(500);	//等待外部电器件上电
	Flag_Init();			//开机相关标志位初始化
	SwitchScreen(0);	//开机首页
	SetTextValue(0,60,"V1.1");	//版本
	Beep_M();	
	
//	DisplayControl(0, 60,0);//隐藏电量电压显示
//	Beep_M();								//初始化完成提示音
	while(1)	
	{
 		if(TFT_USART_RX_Finish==1)//接收到完整的数据，可以进行相应的处理
		{
			if(	(Emergency_Stop_Flag==0)
				||((TFT_USART_RX_BUF[4]==0x02)&&(TFT_USART_RX_BUF[6]==127))
				||((TFT_USART_RX_BUF[4]==0x0A)&&(TFT_USART_RX_BUF[6]==127))
				||TFT_USART_RX_BUF[4]==0x07)//在急停中不能进入程序，系统复位按键可以进去
			{
				if(TFT_USART_RX_BUF[4]==0x00)//【屏幕ID页面,页面0】工作界面
				{
					switch(TFT_USART_RX_BUF[6])
					{
						//【Title】--------------------------------------------------------------------------------------
						case 1://第一排提示
							if(TFT_USART_RX_BUF[9]==1)
							{
								TitleFlag[0]=TFT_USART_RX_BUF[8];
								FLASH_Write(TitleFlag_ADR,(u8 *)&TitleFlag,sizeof(TitleFlag));//保存数据
							}
							break;
						
						case 8://第一排提示
							if(TFT_USART_RX_BUF[9]==1)
							{
								TitleFlag[1]=TFT_USART_RX_BUF[8];
								FLASH_Write(TitleFlag_ADR,(u8 *)&TitleFlag,sizeof(TitleFlag));//保存数据
							}
							break;
							
						case 9://第一排提示
							if(TFT_USART_RX_BUF[9]==1)
							{
								TitleFlag[2]=TFT_USART_RX_BUF[8];
								FLASH_Write(TitleFlag_ADR,(u8 *)&TitleFlag,sizeof(TitleFlag));//保存数据
							}
							break;
							
						case 15://第一排提示
							if(TFT_USART_RX_BUF[9]==1)
							{
								TitleFlag[3]=TFT_USART_RX_BUF[8];
								FLASH_Write(TitleFlag_ADR,(u8 *)&TitleFlag,sizeof(TitleFlag));//保存数据
							}
							break;
							
						//【通道选择】--------------------------------------------------------------------------------------
						case 30:
							if(SelctedChannel[0]==1)
							{//取消选中
								SelctedChannel[0]=0;
								DisplayControl(0,24,0);//隐藏选中
								SwitchReSet(0,30);	//弹起回吸按钮	
							}
							else
							{//选中
								SelctedChannel[0]=1;
								DisplayControl(0,24,1);//选中
								SwitchSet(0,30);	//按下回吸按钮	
							}
							break;
							
						case 31:
							if(SelctedChannel[1]==1)
							{//取消选中
								SelctedChannel[1]=0;
								DisplayControl(0,25,0);//隐藏选中
								SwitchReSet(0,31);	//弹起回吸按钮	
							}
							else
							{//选中
								SelctedChannel[1]=1;
								DisplayControl(0,25,1);//选中
								SwitchSet(0,31);	//按下回吸按钮	
							}
							break;
							
						case 32:
							if(SelctedChannel[2]==1)
							{//取消选中
								SelctedChannel[2]=0;
								DisplayControl(0,26,0);//隐藏选中
								SwitchReSet(0,32);	//弹起回吸按钮
							}
							else
							{//选中
								SelctedChannel[2]=1;
								DisplayControl(0,26,1);//选中
								SwitchSet(0,32);	//按下回吸按钮
							}
							break;
							
						case 33:
							if(SelctedChannel[3]==1)
							{//取消选中
								SelctedChannel[3]=0;
								DisplayControl(0,27,0);//隐藏选中
								SwitchReSet(0,33);	//弹起回吸按钮
							}
							else
							{//选中
								SelctedChannel[3]=1;
								DisplayControl(0,27,1);//选中
								SwitchSet(0,33);	//按下回吸按钮
							}
							break;
						
						//【加液量】--------------------------------------------------------------------------------------
						case 44:
							Add_Volume[0]=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(Add_Volume_ADR,(u8 *)&Add_Volume,sizeof(Add_Volume));//保存数据
							break;
						
						case 45:
							Add_Volume[1]=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(Add_Volume_ADR,(u8 *)&Add_Volume,sizeof(Add_Volume));//保存数据
							break;
						
						case 46:
							Add_Volume[2]=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(Add_Volume_ADR,(u8 *)&Add_Volume,sizeof(Add_Volume));//保存数据
							break;
						
						case 47:
							Add_Volume[3]=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(Add_Volume_ADR,(u8 *)&Add_Volume,sizeof(Add_Volume));//保存数据
							break;
						
						case 13://【运行】--------------------------------------------------------------------------------------
							Systick_Delay_ms(200);							
							SwitchSet(0,13);//按下清洗按钮	
							SetTextValue(0,12,"停止");
							WorkBusy=1;
							switch(WorkMode)
							{
								case 1://[单次加液模式]
									WorkMode_Single();					
									break;
								case 2://[间隔加液模式]
									WorkMode_Interval();
									break;
								case 3://[持续加液模式]
									WorkMode_Continued();
									break;
								
								default:
									break;
							}
							WorkBusy=0;
							Clear_TFT_USART_RX_Buf();
							SetTextValue(0,12,"开始");
							SwitchReSet(0,13);//弹起清洗按钮							
							break;
											
						case 3://【回吸】---------------------------------------------------------------------------------------
							Systick_Delay_ms(200);		
							SwitchSet(0,3);	//按下回吸按钮	
							SetTextValue(0,95,"停止");
							WorkBusy=3;
							BackDraw();	
							WorkBusy=0;
							Clear_TFT_USART_RX_Buf();
							SetTextValue(0,95,"回吸");						
							SwitchReSet(0,3);	//弹起回吸按钮							
							Beep_S();//完成提示音
							break;						
					
						case 2://【填充】---------------------------------------------------------------------------------------			
							Systick_Delay_ms(200);	
							SwitchSet(0,2);//按下填充按钮	
							SetTextValue(0,94,"停止");
							WorkBusy=2;
							FillPipe();
							WorkBusy=0;			
							Clear_TFT_USART_RX_Buf();						
							SetTextValue(0,94,"填充");
							SwitchReSet(0,2);//弹起填充按钮			
							Beep_S();//完成提示音
							break;
						
						case 19://【设置】--------------------------------------------------------------------------------------
							CaSelctedChannel[0]=1;
							CaSelctedChannel[1]=0;
							CaSelctedChannel[2]=0;
							CaSelctedChannel[3]=0;				
							SwitchSet(1,33);	//按下回吸按钮	
							SwitchReSet(1,34);	//弹起回吸按钮	
							SwitchReSet(1,35);	//弹起回吸按钮	
							SwitchReSet(1,36);	//弹起回吸按钮	
							DisplayControl(1, 1,1);//显示子画面2			
							DisplayControl(1, 2,0);//显示子画面2			
							DisplayControl(1, 3,0);//显示子画面2			
							DisplayControl(1, 4,0);//显示子画面2		
							
							for(int i=0;i<4;i++)
							{
								Pump_Theoretically[i]=7000;	//预计填充量
								Pump_Actually[i]=0;					//实际填充量
								
								sprintf(Str,"%1.1f",Pump_Theoretically[i]);	   //类型转换 	
								SetTextValue(3+i,6,Str);
								sprintf(Str,"%1.1f",Pump_Actually[i]);	   //类型转换 
								SetTextValue(3+i,7,Str);
								sprintf(Str,"%1.1f",Pump_OneStep[i]);	   //类型转换 
								SetTextValue(3+i,8,Str);
									
								SetTextValue(3+i,9,"-");
							}
							
							sprintf(Str,"%1.1f",Pump_FillPipe);	   //类型转换 
							SetTextValue(1,25,Str);//填充量
							sprintf(Str,"%1.1f",ALLPumpSpeed);	   //类型转换 
							SetTextValue(1,26,Str);//转速
							
							if(ALLDirFlag==1)
								SwitchReSet(1,27);//弹起回吸按钮	
							else
								SwitchSet(1,27);	//按下回吸按钮	
	
							ScreenState=1;//屏幕进入设置界面
							break;
						
						case 10://【模式选择】----------------------------------------------------------------------------------
							WorkMode++;
							if(WorkMode==4)
								WorkMode=1;
											
							switch(WorkMode)
							{
								case 1:	//1单次加液模式
									SetTextValue(0,17,"单次加液");
									DisplayControl(0, 18,0);//隐藏之画面1		
									for(int i=0;i<4;i++)
									{
										sprintf(Str,"%1.1f",Add_Volume[i]);	 //类型转换
										SetTextValue(0,44+i,Str);	
									}									
									break;
								case 2: //2间隔加液模式
									SetTextValue(0,17,"循环加液");
									DisplayControl(0, 18,1);//显示子画面2				

									Add_Times=0;
									Add_IntervalTime=0;
									sprintf(Str,"%d",Add_Times);	 //类型转换
									SetTextValue(2,3,Str);	
									sprintf(Str,"%d",Add_IntervalTime);	 //类型转换
									SetTextValue(2,4,Str);					
									for(int i=0;i<4;i++)
									{
										sprintf(Str,"%1.1f",Add_Volume[i]);	 //类型转换
										SetTextValue(0,44+i,Str);	
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
							break;
							
						case 11://【方法选择】-----------------------------------------------------------------------------------
							MethodHide_Flag=~MethodHide_Flag;
							if(MethodHide_Flag==0x00)
								DisplayControl(0,23,0);		//隐藏方法选择控件
							else
							{	
								DataRecode_ClearRecode(0,23);//清空数据记录控件
								DisplayControl(0,23,1);		//显示方法选择控件
								
								FLASH_Read(MethodNum_ADR,(u8 *)&MethodNum,sizeof(MethodNum));//读出当前保存的方法总量
								for(int i=0;i<MethodNum;i++)//发送方法名字
								{
									//读取方法名称
									FLASH_Read(Method_ADR+i*sizeof(Method),(u8 *)&Method,sizeof(Method));
									//添加到数据记录控件
									DataRecode_Add1Line(0,23,Method.Name);
								}
							}
							break;

						case 23:
							if(TFT_USART_RX_BUF[2]==0x5A)//确定来自数据记录弹窗
							{
								//读取方法名称
								FLASH_Read(Method_ADR+TFT_USART_RX_BUF[9]*sizeof(Method),(u8 *)&Method,sizeof(Method));
								SetTextValue(0,14,Method.Name);//显示方法名称
								
								//【数据获取】
								WorkMode=Method.WorkMode;
								Add_Times=Method.Add_Times;
								Add_IntervalTime=Method.Add_IntervalTime;								
								for(int i=0;i<4;i++)
								{
									SelctedChannel[i]=Method.SelctedChannel[i];
									TitleFlag[i]=Method.TitleFlag[i];
									Add_Volume[i]=Method.Add_Volume[i];
								}					
								//【数据显示】
								ReadMethod_Show();
								
								//【读出方法】
								MethodHide_Flag=0;
								DisplayControl(0,23,0);		//隐藏方法选择控件								
							}
							break;
							
						case 21://【方法保存】-----------------------------------------------------------------------------------
							SetTextValue(8,6,"方法名称：");	
							SetTextValue(8,5,"*");
							SwitchScreen(8);	//保存弹窗
							ScreenState=3;//屏幕进入设置界面
							break;
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==0x01)//【屏幕ID页面,页面1】设置校准界面
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 33://【校准通道选择】---------------------------------------------------------------------------------------
							CaSelctedChannel[0]=1;
							CaSelctedChannel[1]=0;
							CaSelctedChannel[2]=0;
							CaSelctedChannel[3]=0;				
							SwitchSet(1,33);	//按下回吸按钮	
							SwitchReSet(1,34);	//弹起回吸按钮	
							SwitchReSet(1,35);	//弹起回吸按钮	
							SwitchReSet(1,36);	//弹起回吸按钮	
							DisplayControl(1, 1,1);//显示子画面2			
							DisplayControl(1, 2,0);//显示子画面2			
							DisplayControl(1, 3,0);//显示子画面2			
							DisplayControl(1, 4,0);//显示子画面2			
											
							sprintf(Str,"%1.1f",Pump_Theoretically[0]);	   //类型转换 	
							SetTextValue(3,6,Str);
							sprintf(Str,"%1.1f",Pump_Actually[0]);	   //类型转换 
							SetTextValue(3,7,Str);
							sprintf(Str,"%1.1f",Pump_OneStep[0]);	   //类型转换 
							SetTextValue(3,8,Str);
							
							SetTextValue(3,9,"-");
							break;
						
						case 34:
							CaSelctedChannel[0]=0;
							CaSelctedChannel[1]=1;
							CaSelctedChannel[2]=0;
							CaSelctedChannel[3]=0;				
							SwitchSet(1,34);	//按下回吸按钮	
							SwitchReSet(1,33);	//弹起回吸按钮	
							SwitchReSet(1,35);	//弹起回吸按钮	
							SwitchReSet(1,36);	//弹起回吸按钮	
							DisplayControl(1, 1,0);//显示子画面2			
							DisplayControl(1, 2,1);//显示子画面2			
							DisplayControl(1, 3,0);//显示子画面2			
							DisplayControl(1, 4,0);//显示子画面2	
						
							sprintf(Str,"%1.1f",Pump_Theoretically[1]);	   //类型转换 	
							SetTextValue(4,6,Str);
							sprintf(Str,"%1.1f",Pump_Actually[1]);	   //类型转换 
							SetTextValue(4,7,Str);
							sprintf(Str,"%1.1f",Pump_OneStep[1]);	   //类型转换 
							SetTextValue(4,8,Str);
							
							SetTextValue(4,9,"-");
							break;
						
						case 35:
							CaSelctedChannel[0]=0;
							CaSelctedChannel[1]=0;
							CaSelctedChannel[2]=1;
							CaSelctedChannel[3]=0;				
							SwitchSet(1,35);	//按下回吸按钮	
							SwitchReSet(1,33);	//弹起回吸按钮	
							SwitchReSet(1,34);	//弹起回吸按钮	
							SwitchReSet(1,36);	//弹起回吸按钮	
							DisplayControl(1, 1,0);//显示子画面2			
							DisplayControl(1, 2,0);//显示子画面2			
							DisplayControl(1, 3,1);//显示子画面2			
							DisplayControl(1, 4,0);//显示子画面2	
						
							sprintf(Str,"%1.1f",Pump_Theoretically[2]);	   //类型转换 	
							SetTextValue(5,6,Str);
							sprintf(Str,"%1.1f",Pump_Actually[2]);	   //类型转换 
							SetTextValue(5,7,Str);
							sprintf(Str,"%1.1f",Pump_OneStep[2]);	   //类型转换 
							SetTextValue(5,8,Str);
							
							SetTextValue(5,9,"-");
							break;
						
						case 36:
							CaSelctedChannel[0]=0;
							CaSelctedChannel[1]=0;
							CaSelctedChannel[2]=0;
							CaSelctedChannel[3]=1;				
							SwitchSet(1,36);	//按下回吸按钮	
							SwitchReSet(1,33);	//弹起回吸按钮	
							SwitchReSet(1,35);	//弹起回吸按钮	
							SwitchReSet(1,34);	//弹起回吸按钮	
							DisplayControl(1, 1,0);//显示子画面2			
							DisplayControl(1, 2,0);//显示子画面2			
							DisplayControl(1, 3,0);//显示子画面2			
							DisplayControl(1, 4,1);//显示子画面2	
						
							sprintf(Str,"%1.1f",Pump_Theoretically[3]);	   //类型转换 	
							SetTextValue(6,6,Str);
							sprintf(Str,"%1.1f",Pump_Actually[3]);	   //类型转换 
							SetTextValue(6,7,Str);
							sprintf(Str,"%1.1f",Pump_OneStep[3]);	   //类型转换 
							SetTextValue(6,8,Str);
							
							SetTextValue(6,9,"-");
							break;

						case 26://【转速】---------------------------------------------------------------------------------------------
							ALLPumpSpeed=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(ALLPumpSpeed_ADR,(u8 *)&ALLPumpSpeed,sizeof(ALLPumpSpeed));//保存数据		
							Beep_S();//完成提示音
							break;
						
						case 25://【填充量】---------------------------------------------------------------------------------------------
							Pump_FillPipe=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(Pump_FillPipe_ADR,(u8 *)&Pump_FillPipe,sizeof(Pump_FillPipe));//保存数据
							
							Pump_BackDraw=Pump_FillPipe;
							Pump_ClearPipe=Pump_FillPipe*3;						
							Beep_S();//完成提示音
							break;
						
						case 27://【加液方向】---------------------------------------------------------------------------------------------
							if(ALLDirFlag==1)
							{
								ALLDirFlag=0;
								FLASH_Write(ALLDirFlag_ADR,(u8 *)&ALLDirFlag,sizeof(ALLDirFlag));//保存数据	
								SwitchSet(1,27);	//按下回吸按钮	
							}
							else
							{	
								ALLDirFlag=1;
								FLASH_Write(ALLDirFlag_ADR,(u8 *)&ALLDirFlag,sizeof(ALLDirFlag));//保存数据	
								SwitchReSet(1,27);	//弹起回吸按钮	
							}	
							Beep_S();//完成提示音
							break;
						
						case 6://【恢复默认值】---------------------------------------------------------------------------------------------
							for(int i=0;i<4;i++)
							{
								if(CaSelctedChannel[i]==1)
								{
									Pump_Theoretically[i]=7000;	//预计填充量
									Pump_Actually[i]=0;					//实际填充量
									Pump_OneStep[i]=3400;				//单步进样量
									
									FLASH_Write(Pump_OneStep_ADR,(u8 *)&Pump_OneStep,sizeof(Pump_OneStep));//保存数据	
									
									sprintf(Str,"%1.1f",Pump_Theoretically[i]);	   //类型转换 	
									SetTextValue(3+i,6,Str);
									sprintf(Str,"%1.1f",Pump_Actually[i]);	   //类型转换 
									SetTextValue(3+i,7,Str);
									sprintf(Str,"%1.1f",Pump_OneStep[i]);	   //类型转换 
									SetTextValue(3+i,8,Str);
									
									SetTextValue(3+i,9,"Done!");
								}
							}						
							Beep_S();//完成提示音
							break;
						
						case 28://【后台设置】---------------------------------------------------------------------------------------------			
							sprintf(Str,"%d",SettingParameters.Irun);	   //类型转换 
							SetTextValue(7,6,Str);
							sprintf(Str,"%d",SettingParameters.Ihold);	   //类型转换 
							SetTextValue(7,7,Str);
							ScreenState=3;//屏幕进设置界面
							break;
													
						//【功能键】---------------------------------------------------------------------------------------
						case 29://【填充】---------------------------------------------------------------------------------------------
							Systick_Delay_ms(200);		
							SwitchSet(1,29);//按下填充按钮	
							SetTextValue(1,17,"停止");
							WorkBusy=4;
						
							for(int i=0;i<4;i++)
								{
									if(CaSelctedChannel[i]==1)
									{
										TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
									
										if(ALLDirFlag==1)
											TMC5240_MoveTo(i+1,(Pump_FillPipe*1000.0*1000.0*256/Pump_OneStep[i]));
										else	
											TMC5240_MoveTo(i+1,-(Pump_FillPipe*1000.0*1000.0*256/Pump_OneStep[i]));
										
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
						
							WorkBusy=0;	
							Clear_TFT_USART_RX_Buf();		
							SetTextValue(1,17,"填充");								
							SwitchReSet(1,29);//弹起填充按钮			
							Beep_S();//完成提示音	
							break;
						
						case 30://【加液】---------------------------------------------------------------------------------------------
							Systick_Delay_ms(200);	
							SwitchSet(1,30);//按下控件，蓝色
							SetTextValue(1,18,"停止");	
							WorkBusy=5;

							for(int i=0;i<4;i++)
							{
								if(CaSelctedChannel[i]==1)
								{
									TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
								
									if(ALLDirFlag==1)
										TMC5240_MoveTo(i+1,(Pump_Theoretically[i]*1000.0*256/Pump_OneStep[i]));
									else	
										TMC5240_MoveTo(i+1,-(Pump_Theoretically[i]*1000.0*256/Pump_OneStep[i]));
									
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
							
							WorkBusy=0;
							Clear_TFT_USART_RX_Buf();	
							SetTextValue(1,18,"加液");
							SwitchReSet(1,30);//弹起控件，淡蓝色	
							Beep_S();//完成提示音
							break;
						
						case 31://【清洗】---------------------------------------------------------------------------------------------
							Systick_Delay_ms(200);	
							SwitchSet(1,31);//按下控件，蓝色
							SetTextValue(1,19,"停止");
							WorkBusy=6;

							for(int i=0;i<4;i++)
							{
								if(CaSelctedChannel[i]==1)
								{
									TMC5240_WriteRegister(i+1,TMC5240_VMAX,	ALLPumpSpeed*1197);		//默认240rpm
								
									if(ALLDirFlag==1)
										TMC5240_MoveTo(i+1,(Pump_ClearPipe*1000.0*1000.0*256/Pump_OneStep[i]));
									else	
										TMC5240_MoveTo(i+1,-(Pump_ClearPipe*1000.0*1000.0*256/Pump_OneStep[i]));
									
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
							
							WorkBusy=0;
							Clear_TFT_USART_RX_Buf();	
							SetTextValue(1,19,"清洗");
							SwitchReSet(1,31);//弹起控件，淡蓝色	
							Beep_S();//完成提示音
							break;			
						
						case 32://【返回】--------------------------------------------------------------------------------------------
							ScreenState=0;//屏幕进入工作界面	
							break;
						
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==2)//【屏幕ID页面,子画面1】间隔加液模式子画面
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 3://【加液次数】--------------------------------------------------------------------------------------------
							Add_Times=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						case 4://【间隔时间】--------------------------------------------------------------------------------------------
							Add_IntervalTime=ASCIItoFloat(TFT_USART_RX_BUF);						
							break;
						
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==3)//【屏幕ID页面,子画面Pump1】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 6://【预计加液量】--------------------------------------------------------------------------------------------
							Pump_Theoretically[0]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 7://【实际加液量】--------------------------------------------------------------------------------------------
							Pump_Actually[0]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 5://【校准】--------------------------------------------------------------------------------------------
							//根据预计加液量（由泵管、泵头决定）算出计步数，在根据实际加液量算出单步加液量												
							Systick_Delay_ms(200);	
							SwitchSet(3,5);//弹起填充按钮		
							if(Pump_Actually[0]>0) //实际加液量输入错误
							{		
								Pump_OneStep[0]=(Pump_Actually[0]/(Pump_Theoretically[0]/Pump_OneStep[0]));
								FLASH_Write(Pump_OneStep_ADR,(u8 *)&Pump_OneStep,sizeof(Pump_OneStep));//保存数据	
								
								sprintf(Str,"%d",(uint32_t)Pump_OneStep[0]);	 //类型转换 	
								SetTextValue(3,8,Str);			//显示单步进样量
								SetTextValue(3,9,"Done!");	//显示状态
								SwitchReSet(3,5);						//弹起填充按钮
								break;
							}	
							Beep_S();				//错误提示音
							Systick_Delay_ms(200);
							Beep_S();
							SetTextValue(3,8,"0");		//显示单步进样量单位nL
							SetTextValue(3,9,"Error");//状态栏错误提示	
							SwitchReSet(3,5);//弹起填充按钮				
							break;
						
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==4)//【屏幕ID页面,子画面Pump2】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 6://【预计加液量】--------------------------------------------------------------------------------------------
							Pump_Theoretically[1]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 7://【实际加液量】--------------------------------------------------------------------------------------------
							Pump_Actually[1]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 5://【校准】--------------------------------------------------------------------------------------------
							//根据预计加液量（由泵管、泵头决定）算出计步数，在根据实际加液量算出单步加液量												
							Systick_Delay_ms(200);	
							SwitchSet(4,5);//弹起填充按钮		
							if(Pump_Actually[1]>0) //实际加液量输入错误
							{		
								Pump_OneStep[1]=(Pump_Actually[1]/(Pump_Theoretically[1]/Pump_OneStep[1]));
								FLASH_Write(Pump_OneStep_ADR,(u8 *)&Pump_OneStep,sizeof(Pump_OneStep));//保存数据	
								
								sprintf(Str,"%d",(uint32_t)Pump_OneStep[1]);	 //类型转换 	
								SetTextValue(4,8,Str);			//显示单步进样量
								SetTextValue(4,9,"Done!");	//显示状态
								SwitchReSet(4,5);						//弹起填充按钮
								break;
							}	
							Beep_S();				//错误提示音
							Systick_Delay_ms(200);
							Beep_S();
							SetTextValue(4,8,"0");		//显示单步进样量单位nL
							SetTextValue(4,9,"Error");//状态栏错误提示	
							SwitchReSet(4,5);//弹起填充按钮				
							break;
						
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==5)//【屏幕ID页面,子画面Pump3】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 6://【预计加液量】--------------------------------------------------------------------------------------------
							Pump_Theoretically[2]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 7://【实际加液量】--------------------------------------------------------------------------------------------
							Pump_Actually[2]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 5://【校准】--------------------------------------------------------------------------------------------
							//根据预计加液量（由泵管、泵头决定）算出计步数，在根据实际加液量算出单步加液量												
							Systick_Delay_ms(200);	
							SwitchSet(5,5);//弹起填充按钮		
							if(Pump_Actually[2]>0) //实际加液量输入错误
							{		
								Pump_OneStep[2]=(Pump_Actually[2]/(Pump_Theoretically[2]/Pump_OneStep[2]));
								FLASH_Write(Pump_OneStep_ADR,(u8 *)&Pump_OneStep,sizeof(Pump_OneStep));//保存数据	
								
								sprintf(Str,"%d",(uint32_t)Pump_OneStep[2]);	 //类型转换 	
								SetTextValue(5,8,Str);			//显示单步进样量
								SetTextValue(5,9,"Done!");	//显示状态
								SwitchReSet(5,5);						//弹起填充按钮
								break;
							}	
							Beep_S();				//错误提示音
							Systick_Delay_ms(200);
							Beep_S();
							SetTextValue(5,8,"0");		//显示单步进样量单位nL
							SetTextValue(5,9,"Error");//状态栏错误提示	
							SwitchReSet(5,5);//弹起填充按钮				
							break;
						
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==6)//【屏幕ID页面,子画面Pump4】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 6://【预计加液量】--------------------------------------------------------------------------------------------
							Pump_Theoretically[3]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 7://【实际加液量】--------------------------------------------------------------------------------------------
							Pump_Actually[3]=ASCIItoFloat(TFT_USART_RX_BUF);					
							break;
						
						case 5://【校准】--------------------------------------------------------------------------------------------
							//根据预计加液量（由泵管、泵头决定）算出计步数，在根据实际加液量算出单步加液量												
							Systick_Delay_ms(200);	
							SwitchSet(6,5);//弹起填充按钮		
							if(Pump_Actually[3]>0) //实际加液量输入错误
							{		
								Pump_OneStep[3]=(Pump_Actually[3]/(Pump_Theoretically[3]/Pump_OneStep[3]));
								FLASH_Write(Pump_OneStep_ADR,(u8 *)&Pump_OneStep,sizeof(Pump_OneStep));//保存数据	
								
								sprintf(Str,"%d",(uint32_t)Pump_OneStep[3]);	 //类型转换 	
								SetTextValue(6,8,Str);			//显示单步进样量
								SetTextValue(6,9,"Done!");	//显示状态
								SwitchReSet(6,5);						//弹起填充按钮
								break;
							}	
							Beep_S();				//错误提示音
							Systick_Delay_ms(200);
							Beep_S();
							SetTextValue(6,8,"0");		//显示单步进样量单位nL
							SetTextValue(6,9,"Error");//状态栏错误提示	
							SwitchReSet(6,5);//弹起填充按钮				
							break;
						
						default:
							break;
					}
				}
				if(TFT_USART_RX_BUF[4]==7)//【后台参数】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 6://【电机运行电流】--------------------------------------------------------------------------------------------
							SettingParameters.Irun=ASCIItoFloat(TFT_USART_RX_BUF);
							FLASH_Write(SettingParameters_ADR,(u8 *)&SettingParameters,sizeof(SettingParameters));//保存数据	
							TMC5240_WriteRegister(1,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							TMC5240_WriteRegister(2,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							TMC5240_WriteRegister(3,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							TMC5240_WriteRegister(4,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							break;
						
						case 7://【电机保持电流】--------------------------------------------------------------------------------------------
							SettingParameters.Ihold=ASCIItoFloat(TFT_USART_RX_BUF);		
							FLASH_Write(SettingParameters_ADR,(u8 *)&SettingParameters,sizeof(SettingParameters));//保存数据							
							TMC5240_WriteRegister(1,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							TMC5240_WriteRegister(2,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							TMC5240_WriteRegister(3,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							TMC5240_WriteRegister(4,TMC5240_IHOLD_IRUN,	(0x00060000)|(SettingParameters.Irun<<8)|(SettingParameters.Ihold));//IHOLD_IRUN: IHOLD=10, IRUN=25 (最大电流31), IHOLDDELAY=6
							break;
						
						case 11://【返回】--------------------------------------------------------------------------------------------
							ScreenState=1;//屏幕进设置界面
							break;
						
						default:
							break;
					}
				}
			
				if(TFT_USART_RX_BUF[4]==8)//【方法管理弹窗】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 5://【方法名录入】--------------------------------------------------------------------------------------------
						//判断名称长度是否有效
							if(TFT_USART_CodeLength<=11)
							{
								SetTextValue(8,6,"请输入有效名称");
							}
							if((TFT_USART_CodeLength>11)&&(TFT_USART_CodeLength<=51))
							{
								for(int i=0;i<TFT_USART_CodeLength-11;i++)
								{
									Method.Name[i]=TFT_USART_RX_BUF[i+8];//获取名称
								}
								Method_Name_Unput_Flag=1;
							}
							if(TFT_USART_CodeLength>51)
							{
								SetTextValue(8,6,"名称过长，请重新输入");
							}
							break;
						
						case 3://【确认】--------------------------------------------------------------------------------------------
							if(Method_Name_Unput_Flag==1)//有名称输入
								{
									Method_Name_Unput_Flag=0;						
									if(MethodNum>MaxMethodNum)
									{
										SetTextValue(8,6,"保存方法数量过多！");
										break;
									}
									
									MethodNum++;
									FLASH_Write(MethodNum_ADR,(u8 *)&MethodNum,sizeof(MethodNum));//保存数据	
									Method.WorkMode=WorkMode;
									Method.Add_Times=Add_Times;
									Method.Add_IntervalTime=Add_IntervalTime;
									for(int i=0;i<4;i++)
									{
										Method.SelctedChannel[i]=SelctedChannel[i];
										Method.TitleFlag[i]=TitleFlag[i];
										Method.Add_Volume[i]=Add_Volume[i];
									}
			
									SetTextValue(8,6,"正在保存");	
									FLASH_SaveMethod(Method_ADR,(u8 *)&Method,sizeof(Method),MethodNum);
									SetTextValue(8,6,"保存成功！");	
									Systick_Delay_ms(1000);
									SwitchScreen(0);//保存成功，回到主界面
									ScreenState=0;//屏幕进主界面
								}
							else
								SetTextValue(8,6,"请输入有效名称");	
							break;
							
						case 2://【方法管理】--------------------------------------------------------------------------------------------
							
							//方法数量
							FLASH_Read(MethodNum_ADR,(u8 *)&MethodNum,sizeof(MethodNum));//读出数据	
							sprintf(Str,"%d",MethodNum);	//类型转换 	
							SetTextValue(9,18,Str);			//显示单步进样量
						
							for(int i=0;i<4;i++)
							{
								SetTextValue(9,6+i,"*");	//提示
								SetTextValue(9,10+i,"*");	//加液量
								DisplayControl(9,19+i,0);	//选中提示框
							}
							SetTextValue(9,25,"*");	//加液次数
							SetTextValue(9,26,"*");	//间隔时间
							SetTextValue(9,2,"*");	//方法名
							
							DisplayControl(9,27,0);		//隐藏方法选择控件
							break;	
							
						case 4://【返回】--------------------------------------------------------------------------------------------
							ScreenState=0;//屏幕进主界面
							break;
						
						default:
							break;
					}
				}
				
				if(TFT_USART_RX_BUF[4]==9)//【方法管理界面】
				{
					switch(TFT_USART_RX_BUF[6])
					{
						case 5://【全部删除】--------------------------------------------------------------------------------------------
							MethodNum=0;//方法数量
							FLASH_Write(MethodNum_ADR,(u8 *)&MethodNum,sizeof(MethodNum));//保存数据						
							sprintf(Str,"%d",MethodNum);	//类型转换 	
							SetTextValue(9,18,Str);			//方法数量量
						
							for(int i=0;i<4;i++)
							{
								SetTextValue(9,6+i,"*");	//提示
								SetTextValue(9,10+i,"*");	//加液量
								DisplayControl(9,19+i,0);	//选中提示框
							}
							SetTextValue(9,25,"*");	//提示
							SetTextValue(9,26,"*");	//提示
							SetTextValue(9,2,"*");	//方法名
							Beep_S(); 
							break;		
						
						case 1://【打开/关闭方法】--------------------------------------------------------------------------------------------
							Method_Unput_Flag=~Method_Unput_Flag;
							if(Method_Unput_Flag==0x00)
								DisplayControl(9,27,0);		//隐藏方法选择控件
							if(Method_Unput_Flag==0xFF)
							{	
								DataRecode_ClearRecode(9,27);	//清空数据记录控件
								DisplayControl(9,27,1);				//显示方法选择控件
								for(int i=0;i<MethodNum;i++)//发送方法名字
								{
									//读取方法名称
									FLASH_Read(Method_ADR+sizeof(Method)*i,(u8 *)&Method,sizeof(Method));//读出数据	
									//添加到数据记录控件
									DataRecode_Add1Line(9,27,Method.Name);
								}
							}
							break;
						
						case 27://【选择方法】--------------------------------------------------------------------------------------------
							if(TFT_USART_RX_BUF[2]==0x5A)//确定来自数据记录弹窗
							{
								//【读出方法】
								Method_SwitchFlag=TFT_USART_RX_BUF[9]+1;
								ReadMethodManage_Show(TFT_USART_RX_BUF[9]+1);		
								SetTextValue(9,2,Method.Name);	//方法名
								Method_Unput_Flag=0;
								DisplayControl(9,27,0);		//隐藏方法选择控件
							}
							break;
						
						case 4://【删除当前方法】--------------------------------------------------------------------------------------------
							//没有方法时，直接跳出
							if((MethodNum==0)||(Method_SwitchFlag==0))
							{
								Beep_S();
								Systick_Delay_ms(200);						
								Beep_S(); 
								break;
							}
							FLASH_DeleteMethod(Method_ADR,sizeof(Method),MethodNum, Method_SwitchFlag);
							MethodNum--;//方法数量
							FLASH_Write(MethodNum_ADR,(u8 *)&MethodNum,sizeof(MethodNum));//保存数据		

							for(int i=0;i<4;i++)
							{
								SetTextValue(9,6+i,"*");	//提示
								SetTextValue(9,10+i,"*");	//加液量
								DisplayControl(9,19+i,0);	//选中提示框
							}
							SetTextValue(9,25,"*");	//提示
							SetTextValue(9,26,"*");	//提示
							SetTextValue(9,2,"*");	//方法名
							sprintf(Str,"%d",MethodNum);	//类型转换 	
							SetTextValue(9,18,Str);			//方法数量量
							Beep_S(); 
							break;
							
						case 3://【返回】--------------------------------------------------------------------------------------------
							ScreenState=0;//屏幕进主界面
							break;
						default:
							break;
					}
				}
				
				TFT_USART_RX_Finish=0;//
				if(Emergency_Stop_Flag==1)
				{
					Emergency_Stop_Flag=0;
					WorkBusy=0;//
				}
			}
		}
	}
}

