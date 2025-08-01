#include "HEAD.h"
//存储空间0x0800 0000-0x0807 FFFF,每2K为一页，最小擦除单位是页
//例如：0x0800 0000-0x0800 07FF，为最小页。


/*****************************************************************/
//设置界面相关参数地址
float Flash_PumpSpeed;		//蠕动泵的转速
float Flash_PumpOneStep;	//单步进样量

/*****************************************************************/
//后台日志

/*****************************************************************
* 名    称： FLASH_ReadHalfWord()
* 功    能： 读出一个半字数据，即16位数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
u16 FLASH_ReadHalfWord(uint32_t Address)
{
	return *(__IO uint16_t*)Address; 
}
/*****************************************************************
* 名    称： FLASH_ReadData()
* 功    能： 读出一个32位数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
u32 FLASH_ReadData(u32 StartAddress)
{
	u32 Buffer_Read;
	u32 Buffer_H, Buffer_L;
	
	Buffer_L=FLASH_ReadHalfWord(StartAddress);
	Buffer_H=FLASH_ReadHalfWord(StartAddress+2);
	
	Buffer_Read=(Buffer_H<<16)+Buffer_L;

	return Buffer_Read;
}
 
/*****************************************************************
* 名    称： FLASH_WriteData()
* 功    能： 向内存地址写入一个32位数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
void FLASH_WriteData(u32 FLASH_ADR,u32 Data)
{
	FLASH_Unlock();                    //解锁；以下指令的含义，见“stm32f10X_FLASH.C”
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//清除FLASH_SR寄存器中的BSY,WRPRTERR,EOP标志位
	FLASH_ErasePage(FLASH_ADR); 	     //擦除从FLASH_ADR为起始地址的一页（2kB)
	FLASH_ProgramWord(FLASH_ADR,Data); //写一个字（32位）到从FLASH_ADR为起始地址的4个字节
	FLASH_Lock();					             //加锁
}


/*****************************************************************
* 名    称： FLASH_Read(u32 StartAddress,u8 *Data,u32 DataLength)
* 功    能： 读出数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
void FLASH_Read(u32 StartAddress,u8 *Data,u32 DataLength)
{
	while(DataLength)
	{
		*Data=*(__IO uint8_t*)StartAddress; 
		DataLength--;
		Data++;
		StartAddress++;
	}
	return;
}

/*****************************************************************
* 名    称： FLASH_Write(u32 StartAddress,u8 *Data,u32 DataLength)
* 功    能： 写入数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
void FLASH_Write(u32 StartAddress,u8 *Data,u32 DataLength)
{
	u16 Data16;
	
	FLASH_Unlock();                  //解锁；以下指令的含义，见“stm32f10X_FLASH.C”
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//清除FLASH_SR寄存器中的BSY,WRPRTERR,EOP标志位
	FLASH_ErasePage(StartAddress); 	 //擦除从FLASH_ADR为起始地址的一页（2kB)
	
	for(int i=0;i<(DataLength/2);i++)
	{
		Data16=Data[2*i+1];
		Data16=(Data16<<8)+Data[2*i];
		FLASH_ProgramHalfWord(StartAddress,Data16); //写一个字（32位）到从FLASH_ADR为起始地址的4个字节	
		StartAddress=StartAddress+2;
	}
	if(DataLength%2!=0)
	{
		Data16=((u16)Data[DataLength-1]);
		FLASH_ProgramHalfWord(StartAddress,Data16); //写一个字（32位）到从FLASH_ADR为起始地址的4个字节	
	}
	
	FLASH_Lock();					            //加锁
	return;
}

/*****************************************************************
* 名    称： FLASH_SaveMethod(u32 StartAddress,u8 *Data,u32 DataLength,u32 Number)
* 功    能： 写入数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
void FLASH_SaveMethod(u32 StartAddress,u8 *Data,u32 DataLength,u32 Number)
{
	uint8_t MethodTemp[2048];//临时存储变量
	
	//【读出Flash扇区】
	FLASH_Read(StartAddress,(u8 *)&MethodTemp,sizeof(MethodTemp));
	
	for(int i=0;i<DataLength;i++)
	{
		MethodTemp[(Number-1)*DataLength+i]=Data[i];//Copy
	}

	//【保存Flash扇区】
	FLASH_Write(StartAddress,(u8 *)&MethodTemp,sizeof(MethodTemp));
}

/*****************************************************************
* 名    称： FLASH_DeleteMethod(u32 StartAddress,u8 *Data,u32 DataLength,u32 Number)
* 功    能： 写入数据
* 入口参数： 
* 出口参数： 
 *****************************************************************/
void FLASH_DeleteMethod(u32 StartAddress,u32 DataLength,u32 Sum, u32 Number)
{
	uint8_t MethodTemp[2048];//临时存储变量
	
	//判断是否是最后一个数据
	if(Number==Sum)
		return;
	
	//【读出Flash扇区】
	FLASH_Read(StartAddress,(u8 *)&MethodTemp,sizeof(MethodTemp));

	for(int i=0;i<(DataLength*(Sum-Number));i++)
	{
		MethodTemp[(Number-1)*DataLength+i]=MethodTemp[(Number)*DataLength+i];//后面数据往前移
	}
	
	//【保存Flash扇区】
	FLASH_Write(StartAddress,(u8 *)&MethodTemp,sizeof(MethodTemp));
}
