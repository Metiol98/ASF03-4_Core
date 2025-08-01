#ifndef _BSP_FLASH_H
#define _BSP_FLASH_H
#include "HEAD.h"

#define MaxMethodNum						10
/**********************************************************************************************/
//设置界面相关参数地址
#define	TitleFlag_ADR						0x08060000
#define	SelctedChannel_ADR			0x08060800
#define	Pump_OneStep_ADR				0x08061000
#define	Add_Volume_ADR					0x08061800

#define ALLPumpSpeed_ADR 				0x08062000
#define Pump_FillPipe_ADR				0x08062800
#define ALLDirFlag_ADR					0x08063000

#define SettingParameters_ADR		0x08063800

#define MethodNum_ADR						0x08065000
#define Method_ADR							0x08070000


/******************************4个蠕动泵单步运行进样体积储存地址-宏定义 *******************************/
u16 FLASH_ReadHalfWord(uint32_t Address);			//读取半字，16位数据
u32 FLASH_ReadData(u32 StartAddress);					//读取一个字，32位数据
void FLASH_WriteData(u32 FLASH_ADR,u32 Data);	//写入一个32位数据

void FLASH_Read(u32 StartAddress,u8 *Data,u32 DataLength);
void FLASH_Write(u32 StartAddress,u8 *Data,u32 DataLength);

void FLASH_SaveMethod(u32 StartAddress,u8 *Data,u32 DataLength,u32 Number);
void FLASH_DeleteMethod(u32 StartAddress,u32 DataLength,u32 Sum, u32 Number);
#endif

