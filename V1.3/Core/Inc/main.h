/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include "string.h"
#include "stdio.h"
#include "spi.h"
#include "usart.h"
#include "dma.h"
#include "tim.h"
#include "adc.h"

#include "Function.h"

#include "bsp_Buzzer.h"
#include "bsp_TMC5240.h"
#include "bsp_RingQueue.h"
#include "bsp_LCD.h"
#include "bsp_RFID.h"
#include "bsp_BAT.h"
#include "bsp_W25Qxx.h"

#include "mid_Delay.h"
#include "mid_Flash.h"
#include "mid_Pump.h"
#include "mid_PumpTube.h"
#include "mid_Uart.h"
#include "mid_BasicTIM.h"
#include "mid_Method.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern char 			g_str[];				/* ��ʱ�洢���� */
extern char			  g_str_name[];		/* 临时存储名字变量 */
extern uint8_t 	g_SaveMethodNameFlag; /* 输入保存方法有效 */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI_CS1_Pin GPIO_PIN_0
#define SPI_CS1_GPIO_Port GPIOC
#define SPI_CS2_Pin GPIO_PIN_1
#define SPI_CS2_GPIO_Port GPIOC
#define SPI_CS3_Pin GPIO_PIN_2
#define SPI_CS3_GPIO_Port GPIOC
#define SPI_CS4_Pin GPIO_PIN_3
#define SPI_CS4_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_2
#define RS485_RX_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_3
#define RS485_TX_GPIO_Port GPIOA
#define SPI_SCK_Pin GPIO_PIN_5
#define SPI_SCK_GPIO_Port GPIOA
#define SPI_MISO_Pin GPIO_PIN_6
#define SPI_MISO_GPIO_Port GPIOA
#define SPI_MOSI_Pin GPIO_PIN_7
#define SPI_MOSI_GPIO_Port GPIOA
#define EN1_Pin GPIO_PIN_4
#define EN1_GPIO_Port GPIOC
#define EN2_Pin GPIO_PIN_5
#define EN2_GPIO_Port GPIOC
#define POWER_AD_Pin GPIO_PIN_0
#define POWER_AD_GPIO_Port GPIOB
#define POWER_EN_Pin GPIO_PIN_1
#define POWER_EN_GPIO_Port GPIOB
#define RFID_TX_Pin GPIO_PIN_11
#define RFID_TX_GPIO_Port GPIOB
#define Flash_RST_Pin GPIO_PIN_12
#define Flash_RST_GPIO_Port GPIOB
#define Flash_SCK_Pin GPIO_PIN_13
#define Flash_SCK_GPIO_Port GPIOB
#define Flash_MISO_Pin GPIO_PIN_14
#define Flash_MISO_GPIO_Port GPIOB
#define Flash_MOSI_Pin GPIO_PIN_15
#define Flash_MOSI_GPIO_Port GPIOB
#define EN3_Pin GPIO_PIN_6
#define EN3_GPIO_Port GPIOC
#define EN4_Pin GPIO_PIN_7
#define EN4_GPIO_Port GPIOC
#define ID_1_Pin GPIO_PIN_8
#define ID_1_GPIO_Port GPIOC
#define ID_2_Pin GPIO_PIN_9
#define ID_2_GPIO_Port GPIOC
#define Flash_CS_Pin GPIO_PIN_8
#define Flash_CS_GPIO_Port GPIOA
#define LCD_RX_Pin GPIO_PIN_9
#define LCD_RX_GPIO_Port GPIOA
#define LCD_TX_Pin GPIO_PIN_10
#define LCD_TX_GPIO_Port GPIOA
#define ID_3_Pin GPIO_PIN_10
#define ID_3_GPIO_Port GPIOC
#define ID_4_Pin GPIO_PIN_11
#define ID_4_GPIO_Port GPIOC
#define RFID_MODE_Pin GPIO_PIN_7
#define RFID_MODE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
