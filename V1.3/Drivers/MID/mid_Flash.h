#ifndef __MID_FLASH_H
#define __MID_FLASH_H

#include "main.h"

/************************ FLASH 基本参数 ************************/
#define FLASH_BASE_ADDR        0x08000000U    // FLASH 起始地址     
#define FLASH_TOTAL_PAGES      256U           // 总页数(256KB/2KB)
#define FLASH_END_ADDR         (FLASH_BASE_ADDR + FLASH_TOTAL_PAGES * FLASH_PAGE_SIZE)

// 参数保存页数换算地址
#define PARAM_FLASH_CONFIG_NUM   236U
#define PARAM_FLASH_CONFIG_ADDR  (FLASH_BASE_ADDR + PARAM_FLASH_CONFIG_NUM * FLASH_PAGE_SIZE)

#define PARAM_FLASH_CAIL_NUM   	 238U
#define PARAM_FLASH_CAIL_ADDR 	(FLASH_BASE_ADDR + PARAM_FLASH_CAIL_NUM * FLASH_PAGE_SIZE)

#define PARAM_FLASH_METHOD_NUM   240U
#define PARAM_FLASH_METHOD_ADDR (FLASH_BASE_ADDR + PARAM_FLASH_METHOD_NUM * FLASH_PAGE_SIZE)
#define PARAM_FLASH_PUMPTUBE_NUM   242U
#define PARAM_FLASH_PUMPTUBE_ADDR (FLASH_BASE_ADDR + PARAM_FLASH_PUMPTUBE_NUM * FLASH_PAGE_SIZE)

// 保证参数地址不越界
#if PARAM_FLASH_CONFIG_ADDR + FLASH_PAGE_SIZE > FLASH_END_ADDR
#error "Flash parameter config page address out of range!"
#endif

HAL_StatusTypeDef FLASH_Param_EraseWholePage(uint32_t address);
HAL_StatusTypeDef FLASH_Param_WriteWholePage(uint32_t address,uint8_t *data, uint32_t len);
void FLASH_Param_ReadWholePage(uint32_t address,uint8_t *data, uint32_t len);

HAL_StatusTypeDef FLASH_EraseAutoPages(uint32_t start_addr, uint32_t len);
HAL_StatusTypeDef FLASH_WriteAuto(uint32_t address, uint8_t *data, uint32_t len);
void FLASH_ReadAuto(uint32_t address, uint8_t *data, uint32_t len);

#endif
