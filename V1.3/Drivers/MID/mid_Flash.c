#include "mid_Flash.h"

/**
 * @brief  擦除参数存储专用页（整页擦除）
 * @retval HAL_StatusTypeDef: 操作状态
 */
HAL_StatusTypeDef FLASH_Param_EraseWholePage(uint32_t address)
{
  FLASH_EraseInitTypeDef flash_erase_init;
  uint32_t page_error = 0;

  // 1. 解锁FLASH
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return HAL_ERROR;
  }

  // 2. 配置整页擦除参数
  flash_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;    // 页擦除模式
  flash_erase_init.PageAddress = address;                  // 参数页起始地址
  flash_erase_init.NbPages     = 1;                        // 仅擦除1页

  // 3. 执行擦除
  if (HAL_FLASHEx_Erase(&flash_erase_init, &page_error) != HAL_OK)
  {
    HAL_FLASH_Lock(); // ????,??FLASH
    return HAL_ERROR;
  }

  // 4. 锁定FLASH
  HAL_FLASH_Lock();
  return HAL_OK;
}

/**
 * @brief  向参数页写入数据（整页写入，自动前置擦除）
 * @param  data: 待写入数据缓冲区（长度≤2048字节）
 * @param  len:  写入数据长度（字节数）
 * @retval HAL_StatusTypeDef: 操作状态
 */
HAL_StatusTypeDef FLASH_Param_WriteWholePage(uint32_t address,uint8_t *data, uint32_t len)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t write_addr = address;
  uint16_t half_word = 0;
  uint32_t i = 0;

  // 合法性检查
  if (data == NULL || len == 0 || len > FLASH_PAGE_SIZE)
  {
    return HAL_ERROR; // ????/???0/?????,??????
  }

  // 1. 先擦除整页（FLASH写入前必须擦除）
  status = FLASH_Param_EraseWholePage(write_addr);
  if (status != HAL_OK)
  {
    return status; // 数据为空/长度为0/超过页大小，直接返回错误
  }

  // 2. 解锁FLASH（写入操作需要解锁）
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return HAL_ERROR;
  }

  // 3. 按半字写入（F1 FLASH仅支持半字/字写入，强制对齐）
  for (i = 0; i < len; i += 2)
  {
    // 拼接半字（不足2字节时补0）
    half_word = data[i];
    if (i + 1 < len)
    {
      half_word |= (data[i + 1] << 8);
    }
    else
    {
      half_word |= (0x00 << 8); // 奇数长度补0
    }

    // 写入半字到FLASH
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, write_addr, half_word);
    if (status != HAL_OK)
    {
      break; // 写入失败，终止循环
    }

    // 校验写入数据（可选，增强可靠性）
    if (*(uint16_t *)write_addr != half_word)
    {
      status = HAL_ERROR;
      break;
    }

    write_addr += 2; // 地址偏移2字节（半字）
  }

  // 4. 锁定FLASH
  HAL_FLASH_Lock();
  return status;
}

/**
 * @brief  从参数页读取数据（整页读取）
 * @param  data: 数据存储缓冲区
 * @param  len:  读取长度（字节数，≤2048）
 * @retval void
 */
void FLASH_Param_ReadWholePage(uint32_t address,uint8_t *data, uint32_t len)
{
  uint32_t read_addr = address;
  uint32_t i = 0;

  // 合法性检查
  if (data == NULL || len == 0 || len > FLASH_PAGE_SIZE)
  {
    return; // 非法参数，直接返回
  }

  // 按字节读取FLASH数据（无需解锁）
  for (i = 0; i < len; i++)
  {
    data[i] = *(uint8_t *)read_addr;
    read_addr++;
  }
}

/**
 * @brief  自动计算需要擦除的页数，并擦除
 * @param  start_addr 起始FLASH地址
 * @param  len        数据长度
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef FLASH_EraseAutoPages(uint32_t start_addr, uint32_t len)
{
  FLASH_EraseInitTypeDef erase_init = {0};
  uint32_t page_error = 0;

  uint32_t end_addr = start_addr + len - 1;
  uint32_t start_page = (start_addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  uint32_t end_page   = (end_addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  uint32_t nb_pages   = end_page - start_page + 1;

  uint32_t page_start_addr = FLASH_BASE + start_page * FLASH_PAGE_SIZE;

  if (HAL_FLASH_Unlock() != HAL_OK) return HAL_ERROR;

  erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
  erase_init.PageAddress = page_start_addr;
  erase_init.NbPages     = nb_pages;

  if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK)
  {
    HAL_FLASH_Lock();
    return HAL_ERROR;
  }

  HAL_FLASH_Lock();
  return HAL_OK;
}

/**
 * @brief  任意长度写入，自动跨页，自动擦除
 * @param  address  写入起始地址
 * @param  data     数据指针
 * @param  len      长度
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef FLASH_WriteAuto(uint32_t address, uint8_t *data, uint32_t len)
{
  HAL_StatusTypeDef status = HAL_OK;
//  uint32_t write_addr = address;
//  uint32_t i = 0;
//  uint16_t half_word;

  if (data == NULL || len == 0)
    return HAL_ERROR;

  // ====================== 自动擦除需要的页 ======================
  if(FLASH_EraseAutoPages(address,len) != HAL_OK)
    return HAL_ERROR;

  // 解锁
  if (HAL_FLASH_Unlock() != HAL_OK)
    return HAL_ERROR;

  // // ====================== 按半字写入 ======================
  // for (i = 0; i < len; i += 2)
  // {
  //   // 组半字
  //   half_word = data[i];
  //   if (i + 1 < len)
  //     half_word |= (data[i + 1] << 8);
  //   else
  //     half_word |= 0x00 << 8;  // 奇数长度补0

  //   // 写入
  //   status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, write_addr, half_word);
  //   if (status != HAL_OK)
  //     break;

  //   write_addr += 2;
  // }

	// 按 32 位（WORD）写入 = float 绝对安全，结构体绝对稳定
  uint32_t *src = (uint32_t *)data;
  uint32_t write_cnt = (len + 3) / 4;  // 向上取整为4字节

	for (uint32_t i = 0; i < write_cnt; i++)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, src[i]) != HAL_OK)
    {
      HAL_FLASH_Lock();
      return HAL_ERROR;
    }
    address += 4;
  }

  HAL_FLASH_Lock();
  return status;
}

void FLASH_ReadAuto(uint32_t address, uint8_t *data, uint32_t len)
{
  if (data == NULL || len == 0)
    return;

  uint32_t *dst = (uint32_t *)data;
  uint32_t read_cnt = (len + 3) / 4;

  for (uint32_t i = 0; i < read_cnt; i++)
  {
    dst[i] = *(uint32_t *)address;
    address += 4;
  }
}





































///**
//  * @brief  读出半字数据，即16位数据
//  * @param  Address. 数据地址
//  * @retval None
//  */
//uint16_t FLASH_ReadHalfWord(uint32_t Address)
//{
//	return *(__IO uint16_t *)Address;
//}

///**
//  * @brief  读出一个字数据，即32位数据
//  * @param  Address. 数据地址
//  * @retval None
//  */
//uint32_t FLASH_ReadData(uint32_t Address)
//{
//	uint32_t Buffer_Read;
//	uint32_t Buffer_H, Buffer_L;

//	Buffer_L = FLASH_ReadHalfWord(Address);
//	Buffer_H = FLASH_ReadHalfWord(Address + 2);

//	Buffer_Read = (Buffer_H << 16) + Buffer_L;

//	return Buffer_Read;
//}

///**
//  * @brief  向内存地址写入一个16位数据
//  * @param  Address. 	数据地址
//  * @param  Data. 		要写入的数据
//  * @retval None
//  */
//HAL_StatusTypeDef FLASH_WriteHalfWord(uint32_t Address,uint16_t Data)
//{
//	HAL_StatusTypeDef status = HAL_OK;
//	
//	status = HAL_FLASH_Unlock();                    		//解锁；以下指令的含义，见“stm32f10X_FLASH.C”
//	if(status != HAL_OK)
//	{
//		return status;
//	}
//	
//	FLASH_EraseInitTypeDef erase_init;
//	uint32_t page_error = 0;
//	
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY
//													|FLASH_FLAG_PGERR
//													|FLASH_FLAG_WRPERR
//													|FLASH_FLAG_EOP);			//清除FLASH_SR寄存器中的BSY,WRPRTERR,EOP标志位
//	erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
//	erase_init.PageAddress = Address;
//	erase_init.NbPages	 = 1;
//	erase_init.Banks		 = FLASH_BANK_1;
//	
//	HAL_FLASHEx_Erase(&erase_init,&page_error);
//	
//	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Address,Data);
//	HAL_FLASH_Lock();					             		//加锁
//	
//	if(status == HAL_OK)
//	{
//		uint16_t read_data = *(__IO uint16_t *)Address;
//		if(read_data != Data)
//		{
//			status = HAL_ERROR;
//		}
//	}
//	
//	return status;
//}




























