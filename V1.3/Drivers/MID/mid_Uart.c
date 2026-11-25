#include "mid_Uart.h"

/**
  * @brief  UART扩展接收事件回调函数（支持空闲中断/接收长度阈值事件）
  * @note   当启用UART空闲中断（`HAL_UARTEx_ReceiveToIdle_IT()`/`HAL_UARTEx_ReceiveToIdle_DMA()`）
  *         或接收长度阈值中断时触发，适用于**不定长度数据接收**场景（如串口透传、无固定帧长的通信协议）。
  *         两种触发场景：1. 接收数据达到预设阈值；2. 检测到空闲帧（总线上无数据传输超过1个字符时间），
  *         可通过回调参数获取实际接收的字节数，无需提前知道数据长度。
  * @param  huart  UART外设句柄指针，指向当前触发回调的UART设备（如&huart5）
  * @param  Size   本次接收事件中实际接收到的数据字节数
  * @retval 无返回值
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	/*	RFID	*/
	if(huart->Instance == USART1)
	{
		LCD_IRQ_Process();
	}
	
	/*	LCD	*/
	if(huart->Instance == USART3)
	{
		RFID_IRQ_Process();
	}
}

/**
 * @brief 处理TFT显示屏接收到的命令并执行相应的操作
 */
void TFT_DecodeCommand(uint8_t *arr)
{
	g_TFTPendingCMD.ScreenID = g_TFTPendingCMD.ReadOutArry[3]*256 + g_TFTPendingCMD.ReadOutArry[4];
	g_TFTPendingCMD.ButtonID = g_TFTPendingCMD.ReadOutArry[5]*256 + g_TFTPendingCMD.ReadOutArry[6];

	if( (g_NowStatus.EmergencyStop == USER_FALSE)
		||((g_TFTPendingCMD.ScreenID == 2) && (g_TFTPendingCMD.ButtonID == 127))
		||((g_TFTPendingCMD.ScreenID == 10) && (g_TFTPendingCMD.ButtonID == 127))
		||(g_TFTPendingCMD.ScreenID == 11)
		||(g_TFTPendingCMD.ScreenID == 12)
		||(g_TFTPendingCMD.ScreenID == 13)
		||(g_TFTPendingCMD.ScreenID == 7) )
	{/* 在急停中不能进入程序，系统复位按键可以进去 */
		/* ----------------------------工作界面0，主界面---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 0)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				//【Title】--------------------------------------------------------------------------------------
				case 40://第一排提示
					if((g_TFTPendingCMD.CMDLen >= 12) && (g_TFTPendingCMD.CMDLen <= 51))
					{
						for(int i=0;i<g_TFTPendingCMD.CMDLen-11;i++)
							g_str_name[i] = g_TFTPendingCMD.ReadOutArry[i+8];
						
						strncpy(g_MCBConfig.TitleName[0],g_str_name,METHOD_NAME_SIZE);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					}
					break;
				
				case 41://第一排提示
					if((g_TFTPendingCMD.CMDLen >= 12) && (g_TFTPendingCMD.CMDLen <= 51))
					{
						for(int i=0;i<g_TFTPendingCMD.CMDLen-11;i++)
							g_str_name[i] = g_TFTPendingCMD.ReadOutArry[i+8];
						
						strncpy(g_MCBConfig.TitleName[1],g_str_name,METHOD_NAME_SIZE);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					}
					break;
				
				case 42://第一排提示
					if((g_TFTPendingCMD.CMDLen >= 12) && (g_TFTPendingCMD.CMDLen <= 51))
					{
						for(int i=0;i<g_TFTPendingCMD.CMDLen-11;i++)
							g_str_name[i] = g_TFTPendingCMD.ReadOutArry[i+8];
						
						strncpy(g_MCBConfig.TitleName[2],g_str_name,METHOD_NAME_SIZE);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					}
					break;

				case 43://第一排提示
					if((g_TFTPendingCMD.CMDLen >= 12) && (g_TFTPendingCMD.CMDLen <= 51))
					{
						for(int i=0;i<g_TFTPendingCMD.CMDLen-11;i++)
							g_str_name[i] = g_TFTPendingCMD.ReadOutArry[i+8];
						
						strncpy(g_MCBConfig.TitleName[3],g_str_name,METHOD_NAME_SIZE);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					}
					break;
				
				//【通道选择】--------------------------------------------------------------------------------------
				case 30:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					if(g_NowStatus.SelectedChannel[0]==USER_TRUE)
					{//取消选中
						g_NowStatus.SelectedChannel[0]=USER_FALSE;
						TFTDisplay_DisplayControl(0,24,0);//隐藏选中
						TFTDisplay_DisplayControl(0,70,0);
					}
					else
					{//选中
						g_NowStatus.SelectedChannel[0]=USER_TRUE;
						TFTDisplay_DisplayControl(0,24,1);//选中
						TFTDisplay_DisplayControl(0,70,1);	//按下流量动画按钮	
					}
					break;
				
				case 31:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					if(g_NowStatus.SelectedChannel[1]==USER_TRUE)
					{//取消选中
						g_NowStatus.SelectedChannel[1]=USER_FALSE;
						TFTDisplay_DisplayControl(0,25,0);//隐藏选中
						TFTDisplay_DisplayControl(0,71,0);	//弹起流量动画按钮	
					}
					else
					{//选中
						g_NowStatus.SelectedChannel[1]=USER_TRUE;
						TFTDisplay_DisplayControl(0,25,1);//选中
						TFTDisplay_DisplayControl(0,71,1);	//按下流量动画按钮	
					}
					break;

				case 32:
					TFTDisplay_SetTextValue(0,14,"*");
					if(g_NowStatus.SelectedChannel[2]==USER_TRUE)
					{//取消选中
						g_NowStatus.SelectedChannel[2]=USER_FALSE;
						TFTDisplay_DisplayControl(0,26,0);//隐藏选中
						TFTDisplay_DisplayControl(0,72,0);	//弹起流量动画按钮	
					}
					else
					{//选中
						g_NowStatus.SelectedChannel[2]=USER_TRUE;
						TFTDisplay_DisplayControl(0,26,1);//选中
						TFTDisplay_DisplayControl(0,72,1);	//按下流量动画按钮	
					}
					break;
				
				case 33:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					if(g_NowStatus.SelectedChannel[3]==USER_TRUE)
					{//取消选中
						g_NowStatus.SelectedChannel[3]=USER_FALSE;
						TFTDisplay_DisplayControl(0,27,0);//隐藏选中
						TFTDisplay_DisplayControl(0,73,0);	//弹起流量动画按钮	
					}
					else
					{//选中
						g_NowStatus.SelectedChannel[3]=USER_TRUE;
						TFTDisplay_DisplayControl(0,27,1);//选中
						TFTDisplay_DisplayControl(0,73,1);	//按下流量动画按钮	
					}
					break;

				//【加液量】--------------------------------------------------------------------------------------
				case 44:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					g_MCBConfig.AddVolume[0] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					break;
				
				case 45:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					g_MCBConfig.AddVolume[1] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					break;
				
				case 46:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					g_MCBConfig.AddVolume[2] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					break;
				
				case 47:
					TFTDisplay_SetTextValue(0,14,"*");
					g_MCBConfig.AddVolume[3] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
					break;

				//【运行】--------------------------------------------------------------------------------------
				case 13:
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(0,13,1);//按下按钮	
					TFTDisplay_SetTextValue(0,12,"停止");
					g_NowStatus.WorkingBusy = USER_BUSY;
					switch (WorkMode)
					{
						case SINGLE_MODE://单次加液模式
							WorkMode_Single();
							break;

						case INTERVAL_MODE://间隔加液模式
							WorkMode_Interval();
							break;

						case CONTINUE_MODE://持续加液模式
							WorkMode_Continued();
							break;
						
						default:
							break;
					}
					g_NowStatus.WorkingBusy = USER_FREE;
					TFTDisplay_SetTextValue(0,12,"运行");
					TFTDisplay_SwitchControl(0,13,0);//弹起按钮	
					break;

				//【回吸】---------------------------------------------------------------------------------------
				case 3:
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(0,3,1);//按下回吸按钮	
					TFTDisplay_SetTextValue(0,95,"停止");
					g_NowStatus.WorkingBusy = 3;
					BackSuck();
					g_NowStatus.WorkingBusy = USER_FREE;
					TFTDisplay_SetTextValue(0,95,"回吸");//弹起回吸按钮	
					TFTDisplay_SwitchControl(0,3,0);
					Beep_S();/* 完成提示音 */
					break;

				//【填充】---------------------------------------------------------------------------------------	
				case 2:
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(0,2,1);//按下回吸按钮	
					TFTDisplay_SetTextValue(0,94,"停止");
					g_NowStatus.WorkingBusy = 2;
					FillPipe();
					g_NowStatus.WorkingBusy = USER_FREE;
					TFTDisplay_SetTextValue(0,94,"填充");//弹起回吸按钮	
					TFTDisplay_SwitchControl(0,2,0);
					Beep_S();/* 完成提示音 */
					break;

				//【设置】--------------------------------------------------------------------------------------
				case 19:
				g_NowStatus.CaSelctedChannel[0] = USER_TRUE;
				g_NowStatus.CaSelctedChannel[1] = USER_FALSE;
				g_NowStatus.CaSelctedChannel[2] = USER_FALSE;
				g_NowStatus.CaSelctedChannel[3] = USER_FALSE;
				TFTDisplay_SwitchControl(1,33,1);	//通道1按下
				TFTDisplay_SwitchControl(1,34,0);	//通道2弹起
				TFTDisplay_SwitchControl(1,35,0);	//通道3弹起
				TFTDisplay_SwitchControl(1,36,0);	//通道4弹起
				TFTDisplay_DisplayControl(1,1,1);//显示子画面pump1
				TFTDisplay_DisplayControl(1,2,0);//隐藏子画面pump2
				TFTDisplay_DisplayControl(1,3,0);//隐藏子画面pump3
				TFTDisplay_DisplayControl(1,4,0);//隐藏子画面pump4

				FLASH_Param_ReadWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));
				for (int8_t i = 0; i < 4; i++)
				{
					// g_Cali.Pump_Theoretically[i] = 7000;
					g_Cali.Pump_Actually[i] = 0;

					sprintf(g_str,"%1.1f",g_Cali.Pump_Theoretically[i]);//类型转换
					TFTDisplay_SetTextValue(3+i,6,g_str);
					sprintf(g_str,"%1.1f",g_Cali.Pump_Actually[i]);//类型转换
					TFTDisplay_SetTextValue(3+i,7,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpOneStep[i]);//类型转换
					TFTDisplay_SetTextValue(3+i,8,g_str);

					TFTDisplay_SetTextValue(3+i,9,"-");
				}
				sprintf(g_str,"%1.1f",g_MCBConfig.PipeFillVolume[0]);
				TFTDisplay_SetTextValue(1,25,g_str);//填充量
				sprintf(g_str,"%1.1f",g_MCBConfig.PumpSpeed[0]);
				TFTDisplay_SetTextValue(1,26,g_str);//转速

				if(g_MCBConfig.ALLPumpDir == CCW)
					TFTDisplay_SwitchControl(1,27,0);//弹起回吸按钮	
				else
					TFTDisplay_SwitchControl(1,27,1);//按下回吸按钮

				g_NowStatus.NowScreenPage = Screen_Setting;//屏幕进入设置界面
				TFTDisplay_SwitchScreen(1);
				break;

				//【模式选择】----------------------------------------------------------------------------------
				case 10:
					TFTDisplay_SetTextValue(0,14,"*");	//方法选择名称重置
					WorkMode++;
					if(WorkMode > MODESUM)
						WorkMode = SINGLE_MODE;
					TFTDisplay_SetTextValue(0,14,"*");//在有方法选择的时候重置名字

					switch (WorkMode)
					{
						case SINGLE_MODE://单次加液模式
							TFTDisplay_SetTextValue(0,17,"单次加液");
							TFTDisplay_DisplayControl(0,18,0);//隐藏之画面1
							for (int8_t i = 0; i < 4; i++)
							{
								sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[i]);
								TFTDisplay_SetTextValue(0,44+i,g_str);
								TFTDisplay_DisplayControl(0,4+i,USER_DISPLAY);//显示mL单位
							}
							break;

						case INTERVAL_MODE://间隔加液模式
							TFTDisplay_SetTextValue(0,17,"循环加液");
							TFTDisplay_DisplayControl(0,18,1);//显示子画面2

							g_Interval.AddNumber = 0;
							g_Interval.IntervalTime = 0;
							sprintf(g_str,"%d",g_Interval.AddNumber);
							TFTDisplay_SetTextValue(2,3,g_str);
							sprintf(g_str,"%1.1f",g_Interval.IntervalTime);
							TFTDisplay_SetTextValue(2,4,g_str);
							for (int8_t i = 0; i < 4; i++)
							{
								sprintf(g_str,"%1.2f",g_MCBConfig.AddVolume[i]);
								TFTDisplay_SetTextValue(0,44+i,g_str);
								TFTDisplay_DisplayControl(0,4+i,USER_DISPLAY);//显示mL单位
							}
							break;

						case CONTINUE_MODE://持续加液模式
							TFTDisplay_SetTextValue(0,17,"持续加液");
							TFTDisplay_DisplayControl(0,18,0);//隐藏之画面1
							for (int8_t i = 0; i < 4; i++)
							{
								TFTDisplay_SetTextValue(0,44+i,"-");
								TFTDisplay_DisplayControl(0,4+i,USER_HIDE);//显示mL单位
							}
							break;
						
						default:
							break;
					}
					break;

				case 11://【方法选择】-----------------------------------------------------------------------------------
					g_NowStatus.SaveMethodPressDropMenu = !g_NowStatus.SaveMethodPressDropMenu;
					if(g_NowStatus.SaveMethodPressDropMenu == USER_FALSE)
					{
						TFTDisplay_DisplayControl(0,23,0);//隐藏方法选择控件
						TFTDisplay_DisplayControl(0,22,0);//隐藏控件后面的按钮
					}
					if(g_NowStatus.SaveMethodPressDropMenu == USER_TRUE)
					{
						//先清除记录，在显示记录
						TFTDisplay_DataRecodeClearRecode(0,23);//清空数据记录控件

						for(uint8_t i=0;i<g_MethodCount;i++)
						{
							TFTDisplay_DataRecodeAddLine(0,23,SaveMethod[i].Name);
						}
						TFTDisplay_DisplayControl(0,23,1);//显示方法选择控件
						TFTDisplay_DisplayControl(0,22,1);//显示控件后面的按钮
					}
					break;
				
				case 23:
					if(g_TFTPendingCMD.ReadOutArry[2] == 0x5A)
					{
						g_NowStatus.SaveMethodDropMenuChooseNum = g_TFTPendingCMD.ReadOutArry[9];
						TFTDisplay_SetTextValue(0,14,SaveMethod[g_NowStatus.SaveMethodDropMenuChooseNum].Name);	//方法名称
						Method_Export(g_NowStatus.SaveMethodDropMenuChooseNum);
						Method_RefreshDisplay();
						g_NowStatus.SaveMethodPressDropMenu = USER_FALSE;
						TFTDisplay_DisplayControl(0,23,0);//隐藏方法选择控件
						TFTDisplay_DisplayControl(0,22,0);//隐藏控件后面的按钮
					}
					break;

				case 21:
					TFTDisplay_SetTextValue(8,6,"方法名称：");
					TFTDisplay_SetTextValue(8,5,"*");
					TFTDisplay_SwitchScreen(8);
					g_NowStatus.NowScreenPage=Screen_Parameter;//屏幕进入设置界面
					break;

				default:
				break;
			}
		}

		/* ----------------------------工作界面1，设置校准界面---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 1)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				//【校准通道选择】---------------------------------------------------------------------------------------
				case 33://PUMP1
					FLASH_Param_ReadWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//读取电机基本数据
					FLASH_Param_ReadWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));//读取电机基本数据

					g_NowStatus.CaSelctedChannel[0] = USER_TRUE;
					g_NowStatus.CaSelctedChannel[1] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[2] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[3] = USER_FALSE;
					TFTDisplay_SwitchControl(1,33,1);//按下通道1按钮	
					TFTDisplay_SwitchControl(1,34,0);//弹起通道2按钮
					TFTDisplay_SwitchControl(1,35,0);//弹起通道3按钮
					TFTDisplay_SwitchControl(1,36,0);//弹起通道4按钮
					TFTDisplay_DisplayControl(1,1,1);//显示子画面pump1
					TFTDisplay_DisplayControl(1,2,0);//隐藏子画面pump2
					TFTDisplay_DisplayControl(1,3,0);//隐藏子画面pump3
					TFTDisplay_DisplayControl(1,4,0);//隐藏子画面pump4

					sprintf(g_str,"%1.1f",g_Cali.Pump_Theoretically[0]);//类型转换
					TFTDisplay_SetTextValue(3,6,g_str);
					sprintf(g_str,"%1.1f",g_Cali.Pump_Actually[0]);//类型转换
					TFTDisplay_SetTextValue(3,7,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpOneStep[0]);//类型转换
					TFTDisplay_SetTextValue(3,8,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PipeFillVolume[0]);//类型转换
					TFTDisplay_SetTextValue(1,25,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpSpeed[0]);//类型转换
					TFTDisplay_SetTextValue(1,26,g_str);
					
					TFTDisplay_SetTextValue(3,9,"-");
					break;

				case 34://PUMP2
					FLASH_Param_ReadWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//读取电机基本数据
					FLASH_Param_ReadWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));//读取电机基本数据

					g_NowStatus.CaSelctedChannel[0] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[1] = USER_TRUE;
					g_NowStatus.CaSelctedChannel[2] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[3] = USER_FALSE;
					TFTDisplay_SwitchControl(1,33,0);//按下通道1按钮	
					TFTDisplay_SwitchControl(1,34,1);//弹起通道2按钮
					TFTDisplay_SwitchControl(1,35,0);//弹起通道3按钮
					TFTDisplay_SwitchControl(1,36,0);//弹起通道4按钮
					TFTDisplay_DisplayControl(1,1,0);//显示子画面pump1
					TFTDisplay_DisplayControl(1,2,1);//隐藏子画面pump2
					TFTDisplay_DisplayControl(1,3,0);//隐藏子画面pump3
					TFTDisplay_DisplayControl(1,4,0);//隐藏子画面pump4

					sprintf(g_str,"%1.1f",g_Cali.Pump_Theoretically[1]);//类型转换
					TFTDisplay_SetTextValue(3,6,g_str);
					sprintf(g_str,"%1.1f",g_Cali.Pump_Actually[1]);//类型转换
					TFTDisplay_SetTextValue(3,7,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpOneStep[1]);//类型转换
					TFTDisplay_SetTextValue(3,8,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PipeFillVolume[1]);//类型转换
					TFTDisplay_SetTextValue(1,25,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpSpeed[1]);//类型转换
					TFTDisplay_SetTextValue(1,26,g_str);
					
					TFTDisplay_SetTextValue(3,9,"-");
					break;

				case 35://PUMP3
					FLASH_Param_ReadWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//读取电机基本数据
					FLASH_Param_ReadWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));//读取电机基本数据

					g_NowStatus.CaSelctedChannel[0] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[1] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[2] = USER_TRUE;
					g_NowStatus.CaSelctedChannel[3] = USER_FALSE;
					TFTDisplay_SwitchControl(1,33,0);//按下通道1按钮	
					TFTDisplay_SwitchControl(1,34,0);//弹起通道2按钮
					TFTDisplay_SwitchControl(1,35,1);//弹起通道3按钮
					TFTDisplay_SwitchControl(1,36,0);//弹起通道4按钮
					TFTDisplay_DisplayControl(1,1,0);//显示子画面pump1
					TFTDisplay_DisplayControl(1,2,0);//隐藏子画面pump2
					TFTDisplay_DisplayControl(1,3,1);//隐藏子画面pump3
					TFTDisplay_DisplayControl(1,4,0);//隐藏子画面pump4

					sprintf(g_str,"%1.1f",g_Cali.Pump_Theoretically[2]);//类型转换
					TFTDisplay_SetTextValue(3,6,g_str);
					sprintf(g_str,"%1.1f",g_Cali.Pump_Actually[2]);//类型转换
					TFTDisplay_SetTextValue(3,7,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpOneStep[2]);//类型转换
					TFTDisplay_SetTextValue(3,8,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PipeFillVolume[2]);//类型转换
					TFTDisplay_SetTextValue(1,25,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpSpeed[2]);//类型转换
					TFTDisplay_SetTextValue(1,26,g_str);
					
					TFTDisplay_SetTextValue(3,9,"-");
					break;

				case 36://PUMP4
					FLASH_Param_ReadWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//读取电机基本数据
					FLASH_Param_ReadWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));//读取电机基本数据

					g_NowStatus.CaSelctedChannel[0] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[1] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[2] = USER_FALSE;
					g_NowStatus.CaSelctedChannel[3] = USER_TRUE;
					TFTDisplay_SwitchControl(1,33,0);//按下通道1按钮	
					TFTDisplay_SwitchControl(1,34,0);//弹起通道2按钮
					TFTDisplay_SwitchControl(1,35,0);//弹起通道3按钮
					TFTDisplay_SwitchControl(1,36,1);//弹起通道4按钮
					TFTDisplay_DisplayControl(1,1,0);//显示子画面pump1
					TFTDisplay_DisplayControl(1,2,0);//隐藏子画面pump2
					TFTDisplay_DisplayControl(1,3,0);//隐藏子画面pump3
					TFTDisplay_DisplayControl(1,4,1);//隐藏子画面pump4

					sprintf(g_str,"%1.1f",g_Cali.Pump_Theoretically[3]);//类型转换
					TFTDisplay_SetTextValue(3,6,g_str);
					sprintf(g_str,"%1.1f",g_Cali.Pump_Actually[3]);//类型转换
					TFTDisplay_SetTextValue(3,7,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpOneStep[3]);//类型转换
					TFTDisplay_SetTextValue(3,8,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PipeFillVolume[3]);//类型转换
					TFTDisplay_SetTextValue(1,25,g_str);
					sprintf(g_str,"%1.1f",g_MCBConfig.PumpSpeed[3]);//类型转换
					TFTDisplay_SetTextValue(1,26,g_str);
					
					TFTDisplay_SetTextValue(3,9,"-");
					break;

				//【转速】---------------------------------------------------------------------------------------------
				case 26:
					for (int8_t i = 0; i < 4; i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
						{
							g_MCBConfig.PumpSpeed[i] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
							FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
							Beep_S();
						}
					}
					break;

				//【填充量】---------------------------------------------------------------------------------------------
				case 25:
					for (int8_t i = 0; i < 4; i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
						{
							g_MCBConfig.PipeFillVolume[i] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
							g_MCBConfig.PipeBackDrawVolume[i] = g_MCBConfig.PipeFillVolume[i];
							g_MCBConfig.PipeClearVolume[i] = g_MCBConfig.PipeFillVolume[i] * 3;
							FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));
							Beep_S();
						}
					}
					break;

				//【加液方向】---------------------------------------------------------------------------------------------
				case 27:
					if (g_MCBConfig.ALLPumpDir == CCW)
					{
						g_MCBConfig.ALLPumpDir = CW;
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
						TFTDisplay_SwitchControl(1,27,1);//按下按钮
					}
					else
					{
						g_MCBConfig.ALLPumpDir = CCW;
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
						TFTDisplay_SwitchControl(1,27,0);//弹起按钮
					}
					Beep_S();
					break;

				//【恢复默认值】---------------------------------------------------------------------------------------------
				case 6:
					for(int i=0;i<4;i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
						{
							g_Cali.Pump_Theoretically[i]=7000;	//预计填充量
							g_Cali.Pump_Actually[i]=0;					//实际填充量
							g_MCBConfig.PumpOneStep[i]=5500;		//单步进样量
							g_MCBConfig.PumpSpeed[i] = 260.0;		//默认200RPM
							g_MCBConfig.PipeFillVolume[i] = 5.0;	//默认5ML
							g_MCBConfig.PipeBackDrawVolume[i] = g_MCBConfig.PipeFillVolume[i];
							g_MCBConfig.PipeClearVolume[i] = g_MCBConfig.PipeFillVolume[i] * 3;

							FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
							FLASH_Param_WriteWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));
							
							sprintf(g_str,"%1.1f",g_Cali.Pump_Theoretically[i]);	   //类型转换 	
							TFTDisplay_SetTextValue(3+i,6,g_str);
							sprintf(g_str,"%1.1f",g_Cali.Pump_Actually[i]);	   //类型转换 
							TFTDisplay_SetTextValue(3+i,7,g_str);
							sprintf(g_str,"%1.1f",g_MCBConfig.PumpOneStep[i]);	   //类型转换 
							TFTDisplay_SetTextValue(3+i,8,g_str);
							sprintf(g_str,"%1.1f",g_MCBConfig.PipeFillVolume[i]);//类型转换
							TFTDisplay_SetTextValue(1,25,g_str);
							sprintf(g_str,"%1.1f",g_MCBConfig.PumpSpeed[i]);//类型转换
							TFTDisplay_SetTextValue(1,26,g_str);
							
							TFTDisplay_SetTextValue(3+i,9,"Done!");
						}
					}
					Beep_S();
					break;

				//【后台设置】-------------------------------------------------------------------------------------
				case 28:		
					sprintf(g_str,"%d",g_MCBConfig.Irun);	   //类型转换 
					TFTDisplay_SetTextValue(7,6,g_str);//电机运行电流
					sprintf(g_str,"%d",g_MCBConfig.Ihold);	   //类型转换 
					TFTDisplay_SetTextValue(7,7,g_str);//电机保持电流
					sprintf(g_str,"%d",g_MCBConfig.PipeCallBackVolume[0]);//类型转换 
					TFTDisplay_SetTextValue(7,12,g_str);//PUMP1回调量
					sprintf(g_str,"%d",g_MCBConfig.PipeCallBackVolume[1]);//类型转换 
					TFTDisplay_SetTextValue(7,13,g_str);//PUMP2回调量
					sprintf(g_str,"%d",g_MCBConfig.PipeCallBackVolume[2]);//类型转换 
					TFTDisplay_SetTextValue(7,14,g_str);//PUMP3回调量
					sprintf(g_str,"%d",g_MCBConfig.PipeCallBackVolume[3]);//类型转换 
					TFTDisplay_SetTextValue(7,15,g_str);//PUMP4回调量
					g_NowStatus.NowScreenPage = Screen_Parameter;//屏幕进设置界面
					TFTDisplay_SwitchScreen(7);
					break;

				//【功能键】---------------------------------------------------------------------------------------
				case 29://【填充】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(1,29,1);
					TFTDisplay_SetTextValue(1,17,"停止");
					g_NowStatus.WorkingBusy = 4;

					for (int8_t i = 0; i < 4; i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
							PumpMotion(i+1,CW,g_MCBConfig.PipeFillVolume[i],NoWait);

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
					while(	(Channel1_MotorStatus.Busy==USER_BUSY)
								||(Channel2_MotorStatus.Busy==USER_BUSY)
								||(Channel3_MotorStatus.Busy==USER_BUSY)
								||(Channel4_MotorStatus.Busy==USER_BUSY));

					g_NowStatus.WorkingBusy=USER_FREE;	
					TFTDisplay_SetTextValue(1,17,"填充");
					TFTDisplay_SwitchControl(1,29,0);
					Beep_S();
					break;

				case 30://【加液】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(1,30,1);
					TFTDisplay_SetTextValue(1,18,"停止");
					g_NowStatus.WorkingBusy = 5;

					for (int8_t i = 0; i < 4; i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
							PumpMotion(i+1,CW,g_Cali.Pump_Theoretically[i]/1000,NoWait);
						
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
					while(	(Channel1_MotorStatus.Busy==USER_BUSY)
								||(Channel2_MotorStatus.Busy==USER_BUSY)
								||(Channel3_MotorStatus.Busy==USER_BUSY)
								||(Channel4_MotorStatus.Busy==USER_BUSY));

					SysTickDelay_ms(400);
					for (int8_t i = 0; i < 4; i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
						{
							if(g_MCBConfig.PipeCallBackVolume[i] != 0)
								PumpMotion(i+1,CCW,(float)g_MCBConfig.PipeCallBackVolume[i]/1000,NoWait);
							
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
					while(	(Channel1_MotorStatus.Busy==USER_BUSY)
								||(Channel2_MotorStatus.Busy==USER_BUSY)
								||(Channel3_MotorStatus.Busy==USER_BUSY)
								||(Channel4_MotorStatus.Busy==USER_BUSY));

					g_NowStatus.WorkingBusy=USER_FREE;	
					TFTDisplay_SetTextValue(1,18,"加液");
					TFTDisplay_SwitchControl(1,30,0);
					Beep_S();
					break;

				case 31://【清洗】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(1,31,1);
					TFTDisplay_SetTextValue(1,19,"停止");
					g_NowStatus.WorkingBusy = 6;

					for (int8_t i = 0; i < 4; i++)
					{
						if(g_NowStatus.CaSelctedChannel[i] == USER_TRUE)
							PumpMotion(i+1,CW,g_MCBConfig.PipeClearVolume[i],NoWait);
						
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
					while(	(Channel1_MotorStatus.Busy==USER_BUSY)
								||(Channel2_MotorStatus.Busy==USER_BUSY)
								||(Channel3_MotorStatus.Busy==USER_BUSY)
								||(Channel4_MotorStatus.Busy==USER_BUSY));

					g_NowStatus.WorkingBusy=USER_FREE;	
					TFTDisplay_SetTextValue(1,19,"清洗");
					TFTDisplay_SwitchControl(1,31,0);
					Beep_S();
					break;

				//【返回】--------------------------------------------------------------------------------------------
				case 32:
					g_NowStatus.NowScreenPage = Screen_Working;//屏幕进入工作界面
					TFTDisplay_SwitchScreen(0);
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面2，间隔加液模式子画面---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 2)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				case 3:
					g_Interval.AddNumber = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					break;

				case 4:
					g_Interval.IntervalTime = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					break;

				default:
					break;
			}
		}
		
		/* ----------------------------工作界面3，子画面Pump1---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 3)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				case 6://【预计加液量】
					g_Cali.Pump_Theoretically[0] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));
					break;

				case 7://【实际加液量】
					g_Cali.Pump_Actually[0] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					break;

				case 5://【校准】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(3,5,1);
					if(g_Cali.Pump_Actually[0] > 0)
					{
						g_MCBConfig.PumpOneStep[0] = g_Cali.Pump_Actually[0] / (g_Cali.Pump_Theoretically[0]/g_MCBConfig.PumpOneStep[0]);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据

						sprintf(g_str,"%d",(uint32_t)g_MCBConfig.PumpOneStep[0]);//类型转换
						TFTDisplay_SetTextValue(3,8,g_str);//显示单步进样量
						TFTDisplay_SetTextValue(3,9,"Done!");//显示状态
						TFTDisplay_SwitchControl(3,5,0);//弹起按钮
						break;
					}
					NoticeBeep(250,2);
					TFTDisplay_SetTextValue(3,8,"0");//显示单步进样量
					TFTDisplay_SetTextValue(3,9,"Error!");//显示状态
					TFTDisplay_SwitchControl(3,5,0);//弹起按钮
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面4，子画面Pump2---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 4)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				case 6://【预计加液量】
					g_Cali.Pump_Theoretically[1] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));
					break;

				case 7://【实际加液量】
					g_Cali.Pump_Actually[1] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					break;

				case 5://【校准】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(4,5,1);
					if(g_Cali.Pump_Actually[1] > 0)
					{
						g_MCBConfig.PumpOneStep[1] = g_Cali.Pump_Actually[1] / (g_Cali.Pump_Theoretically[1]/g_MCBConfig.PumpOneStep[1]);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据

						sprintf(g_str,"%d",(uint32_t)g_MCBConfig.PumpOneStep[1]);//类型转换
						TFTDisplay_SetTextValue(4,8,g_str);//显示单步进样量
						TFTDisplay_SetTextValue(4,9,"Done!");//显示状态
						TFTDisplay_SwitchControl(4,5,0);//弹起按钮
						break;
					}
					Beep_S();
					SysTickDelay_ms(200);
					Beep_S();
					TFTDisplay_SetTextValue(4,8,"0");//显示单步进样量
					TFTDisplay_SetTextValue(4,9,"Error!");//显示状态
					TFTDisplay_SwitchControl(4,5,0);//弹起按钮
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面5，子画面Pump3---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 5)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				case 6://【预计加液量】
					g_Cali.Pump_Theoretically[2] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));
					break;

				case 7://【实际加液量】
					g_Cali.Pump_Actually[2] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					break;

				case 5://【校准】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(5,5,1);
					if(g_Cali.Pump_Actually[2] > 0)
					{
						g_MCBConfig.PumpOneStep[2] = g_Cali.Pump_Actually[2] / (g_Cali.Pump_Theoretically[2]/g_MCBConfig.PumpOneStep[2]);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据

						sprintf(g_str,"%d",(uint32_t)g_MCBConfig.PumpOneStep[2]);//类型转换
						TFTDisplay_SetTextValue(5,8,g_str);//显示单步进样量
						TFTDisplay_SetTextValue(5,9,"Done!");//显示状态
						TFTDisplay_SwitchControl(5,5,0);//弹起按钮
						break;
					}
					Beep_S();
					SysTickDelay_ms(200);
					Beep_S();
					TFTDisplay_SetTextValue(5,8,"0");//显示单步进样量
					TFTDisplay_SetTextValue(5,9,"Error!");//显示状态
					TFTDisplay_SwitchControl(5,5,0);//弹起按钮
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面6，子画面Pump4---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 6)
		{
			switch(g_TFTPendingCMD.ButtonID)
			{
				case 6://【预计加液量】
					g_Cali.Pump_Theoretically[3] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CAIL_ADDR,(uint8_t *)&g_Cali,sizeof(g_Cali));
					break;

				case 7://【实际加液量】
					g_Cali.Pump_Actually[3] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					break;

				case 5://【校准】
					SysTickDelay_ms(200);
					TFTDisplay_SwitchControl(3,5,1);
					if(g_Cali.Pump_Actually[3] > 0)
					{
						g_MCBConfig.PumpOneStep[3] = g_Cali.Pump_Actually[3] / (g_Cali.Pump_Theoretically[3]/g_MCBConfig.PumpOneStep[3]);
						FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据

						sprintf(g_str,"%d",(uint32_t)g_MCBConfig.PumpOneStep[3]);//类型转换
						TFTDisplay_SetTextValue(6,8,g_str);//显示单步进样量
						TFTDisplay_SetTextValue(6,9,"Done!");//显示状态
						TFTDisplay_SwitchControl(6,5,0);//弹起按钮
						break;
					}
					Beep_S();
					SysTickDelay_ms(200);
					Beep_S();
					TFTDisplay_SetTextValue(6,8,"0");//显示单步进样量
					TFTDisplay_SetTextValue(6,9,"Error!");//显示状态
					TFTDisplay_SwitchControl(6,5,0);//弹起按钮
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面7，后台参数---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 7)
		{
			switch (g_TFTPendingCMD.ButtonID)
			{
				//【电机运行电流】--------------------------------------------------------------------------------------------
				case 6:
					g_MCBConfig.Irun = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
					TMC5240_WriteReadRegister(1,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					TMC5240_WriteReadRegister(2,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					TMC5240_WriteReadRegister(3,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					TMC5240_WriteReadRegister(4,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					Beep_S();
					break;

				//【电机运行电流】--------------------------------------------------------------------------------------------
				case 7:
					g_MCBConfig.Ihold = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
					TMC5240_WriteReadRegister(1,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					TMC5240_WriteReadRegister(2,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					TMC5240_WriteReadRegister(3,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					TMC5240_WriteReadRegister(4,TMC5240_IHOLD_IRUN,(0X00060000)|(g_MCBConfig.Irun<<8)|(g_MCBConfig.Ihold),TMC_WRITE);
					Beep_S();
					break;

				//【PUMP1回调量】--------------------------------------------------------------------------------------------
				case 12:
					g_MCBConfig.PipeCallBackVolume[0] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
					Beep_S();
					break;

				//【PUMP2回调量】--------------------------------------------------------------------------------------------
				case 13:
					g_MCBConfig.PipeCallBackVolume[1] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
					Beep_S();
					break;

				//【PUMP3回调量】--------------------------------------------------------------------------------------------
				case 14:
					g_MCBConfig.PipeCallBackVolume[2] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
					Beep_S();
					break;

				//【PUMP4回调量】--------------------------------------------------------------------------------------------
				case 15:
					g_MCBConfig.PipeCallBackVolume[3] = TFTDisplay_ASCIItoFloat(g_TFTPendingCMD.ReadOutArry);
					FLASH_Param_WriteWholePage(PARAM_FLASH_CONFIG_ADDR,(uint8_t *)&g_MCBConfig,sizeof(g_MCBConfig));//保存数据
					Beep_S();
					break;

				//【对码模式】--------------------------------------------------------------------------------------------
				case 21:
					g_NowStatus.NowScreenPage = Screen_SRX885Pair;
					TFTDisplay_ClearTextValue(10,6);
					TFTDisplay_SwitchScreen(10);
					break;

				case 22:
					g_NowStatus.NowScreenPage = Screen_PumpTubeManagement;
					PumpTube_RefreshManagementPage();
					TFTDisplay_SwitchScreen(11);
					break;

				//【返回】-------------------------------------------------------------------------------------------------
				case 11:
					g_NowStatus.NowScreenPage = Screen_Setting;//屏幕进设置界面
					TFTDisplay_SwitchScreen(1);
					break;
				
				default:
					break;
			}
		}

		/* ----------------------------工作界面8，方法管理弹窗---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 8)
		{
			switch (g_TFTPendingCMD.ButtonID)
			{
				//【方法名录入】--------------------------------------------------------------------------------------------
				case 5:
					if(g_TFTPendingCMD.CMDLen < 12)
						TFTDisplay_SetTextValue(8,6,"请输入有效名称");

					if((g_TFTPendingCMD.CMDLen >= 12) && (g_TFTPendingCMD.CMDLen <= 51))
					{
						for(int i=0;i<g_TFTPendingCMD.CMDLen-11;i++)
							g_str_name[i] = g_TFTPendingCMD.ReadOutArry[i+8];				//暂存名称，按下确定才将值赋值给存储结构体
						g_SaveMethodNameFlag = USER_TRUE;													//正确获取名称
					}

					if(g_TFTPendingCMD.CMDLen > 51)
						TFTDisplay_SetTextValue(8,6,"名称过长，请重新输入");
					break;

				case 3://【确认】--------------------------------------------------------------------------------------------
					if(g_SaveMethodNameFlag == USER_TRUE)
					{
						TFTDisplay_SetTextValue(8,6,"正在保存...");
						//将当前的加液方法保存到方法结构体中
						if(Method_Add(g_str_name,WorkMode,METHOD_NAME_SIZE))
						{
							TFTDisplay_SetTextValue(8,6,"保存方法数量过多！");
							g_SaveMethodNameFlag = USER_FALSE;
							break;
						}
						g_SaveMethodNameFlag = USER_FALSE;
						TFTDisplay_SetTextValue(8,6,"保存成功！");
						SysTickDelay_ms(1000);
						g_NowStatus.NowScreenPage = Screen_Working;
						TFTDisplay_SwitchScreen(0);//屏幕进主界面
					}
					else
					{
						TFTDisplay_SetTextValue(8,6,"请输入有效名称：");
						NoticeBeep(250,2);
					}
					break;

				//【方法管理】--------------------------------------------------------------------------------------------
				case 2:
					//方法数量
					Flash_MethodLoad();
					g_NowStatus.ManageMethodPressOK = USER_FALSE;
					sprintf(g_str,"%d",g_MethodCount);
					TFTDisplay_SetTextValue(9,18,g_str);
					for(int8_t i=0;i<4;i++)
					{
						TFTDisplay_SetTextValue(9,6+i,"*");
						TFTDisplay_SetTextValue(9,10+i,"*");
						TFTDisplay_DisplayControl(9,19+i,0);
					}
					TFTDisplay_SetTextValue(9,25,"*");
					TFTDisplay_SetTextValue(9,26,"*");
					TFTDisplay_SetTextValue(9,2,"*");
					TFTDisplay_DisplayControl(9,27,0);
					g_NowStatus.ManageMethodPressDropMenu = USER_FALSE;
					TFTDisplay_SwitchScreen(9);
					break;

				//【返回】--------------------------------------------------------------------------------------------
				case 4:
					g_SaveMethodNameFlag = USER_FALSE;
					g_NowStatus.NowScreenPage = Screen_Working;//屏幕进主界面
					TFTDisplay_SwitchScreen(0);
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面9，方法管理界面---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 9)
		{
			switch (g_TFTPendingCMD.ButtonID)
			{
				//【全部删除】--------------------------------------------------------------------------------------------
				case 5:
					if(g_MethodCount > 0)
					{
						Method_DeletAll();
						Flash_MethodLoad();
						Beep_S();
					}
					else
					{
						NoticeBeep(250,2);
					}
					sprintf(g_str,"%d",g_MethodCount);
					TFTDisplay_SetTextValue(9,18,g_str);
					for(int8_t i=0;i<4;i++)
					{
						TFTDisplay_SetTextValue(9,6+i,"*");
						TFTDisplay_SetTextValue(9,10+i,"*");
						TFTDisplay_DisplayControl(9,19+i,0);
					}
					TFTDisplay_SetTextValue(9,25,"*");
					TFTDisplay_SetTextValue(9,26,"*");
					TFTDisplay_SetTextValue(9,2,"*");
					g_NowStatus.ManageMethodPressOK = USER_FALSE;
					break;
				
				//【打开/关闭方法】--------------------------------------------------------------------------------------------
				case 1:
					g_NowStatus.ManageMethodPressDropMenu = ~g_NowStatus.ManageMethodPressDropMenu;
					if(g_NowStatus.ManageMethodPressDropMenu == USER_FALSE)
					{
						TFTDisplay_DisplayControl(9,27,0);
					}
					else
					{
						//先清空记录，再显示记录
						TFTDisplay_DataRecodeClearRecode(9,27);
						TFTDisplay_DisplayControl(9,27,1);
						for (int8_t i = 0; i < g_MethodCount; i++)
						{
							TFTDisplay_DataRecodeAddLine(9,27,SaveMethod[i].Name);
						}
					}
					break;

				//【选择方法】--------------------------------------------------------------------------------------------
				case 27:
					if(g_TFTPendingCMD.ReadOutArry[2] == 0x5A)
					{
						g_NowStatus.ManageMethodDropMenuChooseNum = g_TFTPendingCMD.ReadOutArry[9];
						Method_ManageShow(g_NowStatus.ManageMethodDropMenuChooseNum);
						TFTDisplay_SetTextValue(9,2,SaveMethod[g_NowStatus.ManageMethodDropMenuChooseNum].Name);
						g_NowStatus.ManageMethodPressDropMenu = USER_FALSE;
						TFTDisplay_DisplayControl(9,27,0);
						g_NowStatus.ManageMethodPressOK = USER_TRUE;
					}
					break;

				//【删除当前方法】--------------------------------------------------------------------------------------------
				case 4:
					if(g_NowStatus.ManageMethodPressOK == USER_TRUE)
					{
						if((!Method_Delete(g_NowStatus.ManageMethodDropMenuChooseNum)))
						{
							Flash_MethodLoad();
							Beep_M();
							SysTickDelay_ms(1000);
						}
						else
						{
							NoticeBeep(250,2);
							SysTickDelay_ms(1000);
							break;
						}
						sprintf(g_str,"%d",g_MethodCount);
						TFTDisplay_SetTextValue(9,18,g_str);
						for(int8_t i=0;i<4;i++)
						{
							TFTDisplay_SetTextValue(9,6+i,"*");
							TFTDisplay_SetTextValue(9,10+i,"*");
							TFTDisplay_DisplayControl(9,19+i,0);
						}
						TFTDisplay_SetTextValue(9,25,"*");
						TFTDisplay_SetTextValue(9,26,"*");
						TFTDisplay_SetTextValue(9,2,"*");
						g_NowStatus.ManageMethodPressOK = USER_FALSE;
					}
					else
					{
						//提示没有选择方法
						NoticeBeep(250,2);
					}
					break;

				//【返回】--------------------------------------------------------------------------------------------
				case 3:
					g_NowStatus.NowScreenPage = Screen_Working;
					TFTDisplay_SwitchScreen(0);
					g_NowStatus.ManageMethodPressOK = USER_FALSE;
					break;

				default:
					break;
			}
		}

		/* ----------------------------工作界面11，泵管管理界面---------------------------- */
		if ((g_TFTPendingCMD.ScreenID == 11U) &&
		    (g_NowStatus.NowScreenPage == Screen_PumpTubeManagement))
		{
			uint16_t control = g_TFTPendingCMD.ButtonID;
			if (control == 1U)
			{
				PumpTube_SetEnabled((uint8_t)!PumpTube_IsEnabled());
				(void)PumpTube_SaveNow();
				Beep_S();
				PumpTube_RefreshManagementPage();
			}
			else if (control == 42U)
			{
				g_NowStatus.NowScreenPage = Screen_Setting;
				TFTDisplay_SwitchScreen(7U);
			}
			else if (control == 16U)
				PumpTube_OpenRegistration();
			else
				PumpTube_HandleManagementInput(control, arr, g_TFTPendingCMD.CMDLen);
		}
		
		/* ----------------------------工作界面13，泵管登记更换时间---------------------------- */
		if ((g_TFTPendingCMD.ScreenID == 13U) &&
		    (g_NowStatus.NowScreenPage == Screen_PumpTubeRegistration))
			PumpTube_HandleRegistration(g_TFTPendingCMD.ButtonID, arr, g_TFTPendingCMD.CMDLen);

		/* ----------------------------工作界面12，泵管提示弹窗---------------------------- */
		if ((g_TFTPendingCMD.ScreenID == 12) && (g_TFTPendingCMD.ButtonID == 7U))
		{
			g_NowStatus.NowScreenPage = Screen_Working;
			TFTDisplay_SwitchScreen(0U);
		}

		/* ----------------------------工作界面10，对码管理界面---------------------------- */
		if( g_TFTPendingCMD.ScreenID == 10)
		{
			switch (g_TFTPendingCMD.ButtonID)
			{
				//【进入对码模式】--------------------------------------------------------------------------------------------
				case 2:
					g_NowStatus.SRX885PairMode = USER_TRUE;
					TFTDisplay_SetTextValue(10,6,"对码中...");
					RFID_Mode_PairMode();
					break;

				//【清除所有对码】--------------------------------------------------------------------------------------------
				case 4:
					TFTDisplay_SetTextValue(10,6,"正在清除所有对码！！！");
					RFID_Mode_ClearAll();
					TFTDisplay_SetTextValue(10,6,"清除成功！！！");
					Beep_M();
					SysTickDelay_ms(1000);
					TFTDisplay_ClearTextValue(10,6);
					break;

				//【返回】--------------------------------------------------------------------------------------------
				case 7:
					g_NowStatus.NowScreenPage = Screen_Setting;
					TFTDisplay_SwitchScreen(7);
					break;

				default:
					break;
			}
		}
	}
	g_TFTPendingCMD.PendingCMDFlag = USER_FALSE; // 清除命令标志位
	if (g_NowStatus.EmergencyStop == USER_TRUE)
	{
		g_NowStatus.EmergencyStop = USER_FALSE;
		g_NowStatus.WorkingBusy = USER_FREE;
	}
	
}







































