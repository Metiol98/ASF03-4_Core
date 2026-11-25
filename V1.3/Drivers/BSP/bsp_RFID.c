/**************************************************************************************************
 * @file         bsp_RFID.c
 * @brief        USART驱动的SRX885模块
 * @note         通用驱动层，与硬件平台无关
 *               硬件相关配置请查看 bsp_RFID.h
 *
 * @attention
 *
 * 1. 本文件为通用驱动，不包含硬件引脚定义，移植时只需修改 bsp_RFID.h
 * 2. 驱动仅实现寄存器操作、使能控制、运行逻辑，不依赖具体硬件
 * 3. 更换硬件平台时，此文件无需修改
 *
 * @author       Q1uuuu
 * @date         2026-04-15
 * @version      V1.0
 *************************************************************************************************/

#include "bsp_RFID.h"

uint8_t RfidRx_Finish = 0;
uint8_t g_rfid_Rx_Buffer[RfidRxBufferSize] = {'\0'};

struct RFID
{
	uint8_t Header[4];
	uint8_t RFID_ID[9];
}RFID_DATA;

void RFID_init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3,g_rfid_Rx_Buffer,RfidRxBufferSize);		//开启huart2 dma中断
}

uint8_t Data_Right_Handler(void)
{
	uint8_t right_flag = 1;
	
	/* 1.判断长度 */
	if(strlen((char*)g_rfid_Rx_Buffer) != 13)
	{
		right_flag=0;
	}
	
	/* 2.判断数字前的冒号位置 */
	if(g_rfid_Rx_Buffer[2] != ':')
	{
		right_flag=0;
	}
	
	if(g_rfid_Rx_Buffer[11]!=0x0D || g_rfid_Rx_Buffer[12]!=0x0A)
	{
		right_flag=0;
	}
	
	//memset(g_rfid_Rx_Buffer,0,sizeof(g_rfid_Rx_Buffer));
	return right_flag;
}

void RFID_Run(void)
{
	if(RfidRx_Finish == 1)
	{
		RfidRx_Finish = 0;
		
	}
}

/**
  * @brief  SRX885串口中断函数，串口空闲中断进入此函数，做数据判断处理；
  * @param  None.
  * @retval None.
  */
void RFID_IRQ_Process(void)
{
	if(Data_Right_Handler())						/* 正确的有效指令,按键有效，数据处理 */
	{
		g_NowStatus.SRX885Counter = 0;				 // 计数归零，防止按键粘连重复进入，
		if (g_NowStatus.SRX885OK == USER_TRUE) // 可以重新进入按键程序
		{
			g_NowStatus.SRX885OK = USER_FALSE;				// 可以重新进入标志位归零
			if (g_NowStatus.WorkingBusy == USER_FREE) // 空闲时按下
			{
				if (g_NowStatus.NowScreenPage == Screen_Working) 		// 0在工作界面，1在校准界面。
				{
					g_TFTPendingCMD.ReadOutArry[4] = 0; 			// 模拟屏幕的运行键按下
					g_TFTPendingCMD.ReadOutArry[6] = 13;
					g_TFTPendingCMD.PendingCMDFlag = USER_TRUE; // 有指令待处理
				}
				if (g_NowStatus.NowScreenPage == Screen_Setting) 			// 0在工作界面，1在校准界面。
				{
					g_TFTPendingCMD.ReadOutArry[4] = 1; 			// 模拟屏幕的运行键按下
					g_TFTPendingCMD.ReadOutArry[6] = 30;
					g_TFTPendingCMD.PendingCMDFlag = USER_TRUE; // 有指令待处理
				}
			}
			if (g_NowStatus.WorkingBusy != USER_FREE) // 忙碌时按下
			{
				Emergency_Stop();
				g_Interval.InterTimeCNT = 0;
				g_Interval.InterTimeOver = USER_TRUE;
				g_NowStatus.EmergencyStop = USER_TRUE;
			}
		}
		if((g_NowStatus.SRX885PairMode == USER_TRUE) && (g_NowStatus.NowScreenPage == Screen_SRX885Pair))	/* 在对码模式下*/
		{
			/* 配对成功 */
			g_NowStatus.SRX885PairMode = USER_FALSE;	// 退出对码模式
		}
	}
	
	HAL_UARTEx_ReceiveToIdle_DMA(&RFID_UART,g_rfid_Rx_Buffer,RfidRxBufferSize);
}

/**
  * @brief  SRX885
  * @param  None.
  * @retval None.
  */
void RFID_Mode_BackupInpuConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = RFID_MODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	
  HAL_GPIO_Init(RFID_MODE_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  RFID设置成输出模式
 * @param  None.
 * @retval None.
 */
void RFID_Mode_SetOutputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = RFID_MODE_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

    // 
    HAL_GPIO_WritePin(RFID_MODE_GPIO_Port, RFID_MODE_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  RFID双击设置，双击进入配对模式
 * @param  None.
 * @retval None.
 */
void RFID_Mode_DoubleClick(void)
{
	RFID_Mode_SetOutputMode();
	SysTickDelay_ms(50);
	RFID_Mode_BackupInpuConfig();
	SysTickDelay_ms(200);
	RFID_Mode_SetOutputMode();
	SysTickDelay_ms(50);
	RFID_Mode_BackupInpuConfig();
}

/**
 * @brief  RFID长按设置，长按6s清除所有对码
 * @param  None.
 * @retval None.
 */
void RFID_Mode_ClearAll(void)
{
	RFID_Mode_SetOutputMode();
	SysTickDelay_ms(7000);
	RFID_Mode_BackupInpuConfig();
}

/**
 * @brief  RFID短按1s，退出对码模式（仅在对码模式下有效）
 * @param  None.
 * @retval None.
 */
void RFID_Mode_ShortPress(void)
{
	RFID_Mode_SetOutputMode();
	SysTickDelay_ms(1200);
	RFID_Mode_BackupInpuConfig();
}

/**
 * @brief  RFID对码模式
 * @param  None.
 * @retval None.
 */
void RFID_Mode_PairMode(void)
{
	const uint32_t Pair_Timeout = 10000;	// 10s超时
	SRX885_Pair_Status Pair_Status = SRX885_PAIR_UNKNOWN;

	uint32_t StartTime = HAL_GetTick();

	RFID_Mode_DoubleClick();	// 进入对码模式
	TFTDisplay_SetTextValue(10,6,"请按压手柄进行对码...");
	while (Pair_Status != SRX885_PAIR_SUCCESS)
	{
		if (g_NowStatus.SRX885PairMode == USER_FALSE)
		{
			Pair_Status = SRX885_PAIR_SUCCESS;	// 对码成功
			TFTDisplay_SetTextValue(10,6,"对码成功"); 
			SysTickDelay_ms(1000);
			g_NowStatus.NowScreenPage = Screen_Setting;		//对码成功以后会退出弹窗，防止在设置界面无法通过手柄控制加液
			Beep_S();	// 提示音
			TFTDisplay_SwitchScreen(7);	/* 屏幕跳转 */
			break;
		}

		if ((HAL_GetTick() - StartTime) >= Pair_Timeout)
		{
			Pair_Status = SRX885_PAIR_TIMEOUT;	// 对码超时
			TFTDisplay_SetTextValue(10,6,"对码超时");
			NoticeBeep(250, 2);
			break;
		}
	}
	
}





