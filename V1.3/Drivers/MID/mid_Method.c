#include "mid_Method.h"

volatile uint8_t g_MethodCount = 0;

/**
  * @brief  方法列表初始化
  */
void Method_Init(void)
{
	//从FLASH中获取方法
	Flash_MethodLoad();
	
	// 如果Flash中没有有效数据，初始化空列表
	if(g_MethodCount == 0)
	{
		for(uint8_t i=0;i<MAX_METHOD_COUNT;i++)
		{
			SaveMethod[i].valid = 0;
			memset(SaveMethod[i].Name,0,sizeof(SaveMethod[i].Name));
		}
	}
}

/*-------------         方法保存相关函数分界线       ---------------*/
/**
  * @brief  方法保存到Flash
  * @param  None.
  * @retval None
  */
void Flash_MethodSave(void)
{
	//FLASH_Param_WriteWholePage(PARAM_FLASH_METHOD_ADDR,(uint8_t*)SaveMethod,sizeof(SaveMethod));
	FLASH_WriteAuto(PARAM_FLASH_METHOD_ADDR,(uint8_t*)SaveMethod,sizeof(SaveMethod));
}

/**
  * @brief  方法从Flash加载
  * @param  name:    方法名字
  * @param  mode: 	 工作模式
  * @retval None
  */
void Flash_MethodLoad(void)
{
	// FLASH_Param_ReadWholePage(PARAM_FLASH_METHOD_ADDR,(uint8_t*)SaveMethod,sizeof(SaveMethod));
	FLASH_ReadAuto(PARAM_FLASH_METHOD_ADDR,(uint8_t*)SaveMethod,sizeof(SaveMethod));

	g_MethodCount = 0;
	
	for(uint8_t i=0;i<MAX_METHOD_COUNT;i++)
	{
		if(SaveMethod[i].valid == USER_TRUE)
			g_MethodCount++;
	}
}

/**
  * @brief  添加新方法
  * @param  name:    方法名字
  * @param  mode: 	 工作模式
  * @retval None
  */
uint8_t Method_Add(char* name, uint8_t mode,uint32_t size) 
{
	// 检查是否已达最大数量
	if(g_MethodCount >= MAX_METHOD_COUNT)
		return 1;		// 方法数量已满
	
	// 找到空位置并添加方法
	for (uint8_t i = 0; i < MAX_METHOD_COUNT; i++) 
	{
		if(SaveMethod[i].valid != USER_TRUE)
		{
			SaveMethod[i].valid = USER_TRUE;
			//保存名字
			strncpy(SaveMethod[i].Name,name,size);
			SaveMethod[i].Mode = mode;
			SaveMethod[i].Add_Times = g_Interval.AddNumber;
			SaveMethod[i].Add_IntervalTime = g_Interval.IntervalTime;

			for (int8_t j = 0; j < 4; j++)
			{
				memcpy(SaveMethod[i].TitleName[j], g_MCBConfig.TitleName[j], METHOD_NAME_SIZE);
				SaveMethod[i].SelctedChannel[j] = g_NowStatus.SelectedChannel[j];
				SaveMethod[i].Add_Volume[j] = g_MCBConfig.AddVolume[j];
			}
			
			g_MethodCount++;
			//添加保存到Flash中的函数
			Flash_MethodSave();
			
			return 0;	//添加成功
		}
	}
	
	return 1;	//添加失败
}


/**
  * @brief  删除指定索引的方法
  * @param  index:    方法索引
  * @retval None
  */
 uint8_t Method_Delete(uint8_t index) 
{
	if((index >= MAX_METHOD_COUNT) ||	!(SaveMethod[index].valid)) 
	{
		return 1;  // 索引无效或方法已删除
  }
	
	//将删除数据后面的依次往前挪动
	uint8_t current = index;		//当前需要填充的位置
	
	for(uint8_t i=index+1;i<MAX_METHOD_COUNT;i++)
	{
		if(SaveMethod[i].valid)
		{
			SaveMethod[current] = SaveMethod[i];	
			current++;
			
			//如果是最后一个有效方法，后面的位置标记为无效
			if(i == MAX_METHOD_COUNT -1)
			{
				SaveMethod[current].valid = 0;
				memset(SaveMethod[current].Name,0,sizeof(SaveMethod[current].Name));
			}
		}
		else
		{
			//遇到无效方法，代表后面没有数据，直接清空当前位置
			SaveMethod[current].valid = 0;
			memset(SaveMethod[current].Name,0,sizeof(SaveMethod[current].Name));
			break;
		}
	}
	
	g_MethodCount--;
	
	//添加保存到Flash中的函数
	Flash_MethodSave();
			
	return 0;	//删除成功
}

/**
  * @brief  删除所有方法
  */
void Method_DeletAll(void)
{
	for(uint8_t i=0;i<MAX_METHOD_COUNT;i++)
	{
		SaveMethod[i].valid = USER_FALSE;
		memset(SaveMethod[i].Name,0,sizeof(SaveMethod[i].Name)); 
	}
	g_MethodCount = 0;
	
	//添加保存到Flash中的函数
	Flash_MethodSave();
}

/**
  * @brief  导出指定方法
  * @param  index:    方法索引
  * @retval None
  */
uint8_t Method_Export(uint8_t index) 
{
	if((index >= MAX_METHOD_COUNT) ||	!(SaveMethod[index].valid))
	{
		return 1;//索引无效
	}
	
	SaveMethodSTR* method = &SaveMethod[index];
	
  WorkMode = method->Mode;
  g_Interval.AddNumber = method->Add_Times;
  g_Interval.IntervalTime = method->Add_IntervalTime;
  for (int8_t i = 0; i < 4; i++)
  {
    g_NowStatus.SelectedChannel[i] = method->SelctedChannel[i];
		memcpy(g_MCBConfig.TitleName[i], method->TitleName[i], METHOD_NAME_SIZE);
    g_MCBConfig.AddVolume[i] = method->Add_Volume[i];
  }

	return 0;  // 导出成功
}

void Method_ManageShow(uint8_t index)
{
	Flash_MethodLoad();

	for (int8_t i = 0; i < 4; i++)
	{
		// TFTDisplay_SendTitleTextValue(9,6+i,SaveMethod[index].TitleName[i]);//通道提示
		TFTDisplay_SetTextValue(9,6+i,SaveMethod[index].TitleName[i]);
		sprintf(g_str,"%1.2f",SaveMethod[index].Add_Volume[i]);//类型转换
		TFTDisplay_SetTextValue(9,10+i,g_str);//【通道加液量】
		if(SaveMethod[index].SelctedChannel[i] == USER_TRUE)
		{
			TFTDisplay_DisplayControl(9,19+i,USER_TRUE);//通道选中提示
		}
		else
		{
			TFTDisplay_DisplayControl(9,19+i,USER_FALSE);//通道不选中提示	
			TFTDisplay_SetTextValue(9,10+i,"0");
		}
	}

	if(SaveMethod[index].Mode == 1)
	{
		TFTDisplay_SetTextValue(9,25,"*");
		TFTDisplay_SetTextValue(9,26,"*");
	}
	else if (SaveMethod[index].Mode == 2)
	{
		sprintf(g_str,"%d",SaveMethod[index].Add_Times);	 //类型转换
		TFTDisplay_SetTextValue(9,25,g_str);	
		sprintf(g_str,"%1.1f",SaveMethod[index].Add_IntervalTime);	 //类型转换
		TFTDisplay_SetTextValue(9,26,g_str);
	}
	else if (SaveMethod[index].Mode == 3)
	{
		for (int8_t j = 0; j < 4; j++)
		{
			TFTDisplay_SetTextValue(9,10+j,"-");
		}
		
	}
	

	
}




































