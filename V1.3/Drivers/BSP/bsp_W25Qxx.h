#ifndef __BSP_W25QXX_H__
#define __BSP_W25QXX_H__

#include "main.h"

/************************ 硬件配置（根据实际硬件修改） ************************/
#define W25QX_SPI_HANDLE    &hspi2

#define Flash_WP_GPIO_Port	GPIOB
#define Flash_WP_Pin				GPIO_PIN_10

#define W25QX_CS_HIGH()     HAL_GPIO_WritePin(Flash_CS_GPIO_Port, Flash_CS_Pin, GPIO_PIN_SET)
#define W25QX_CS_LOW()      HAL_GPIO_WritePin(Flash_CS_GPIO_Port, Flash_CS_Pin, GPIO_PIN_RESET)
#define W25QX_WP_HIGH()     HAL_GPIO_WritePin(Flash_WP_GPIO_Port, Flash_WP_Pin, GPIO_PIN_SET)
#define W25QX_WP_LOW()      HAL_GPIO_WritePin(Flash_WP_GPIO_Port, Flash_WP_Pin, GPIO_PIN_RESET)
#define W25QX_HOLD_HIGH()   HAL_GPIO_WritePin(Flash_RST_GPIO_Port, Flash_RST_Pin, GPIO_PIN_SET)
#define W25QX_HOLD_LOW()    HAL_GPIO_WritePin(Flash_RST_GPIO_Port, Flash_RST_Pin, GPIO_PIN_RESET)

/*  W25Q128参数定义 */
#define W25Q128_PAGE_SIZE         256     // 每页256字节
#define W25Q128_SECTOR_SIZE       4096    // 每扇区4096字节
#define W25Q128_BLOCK_SIZE        65536   // 每块64K字节
#define W25Q128_TOTAL_SIZE        16*1024*1024 // 总容量128Mbit = 16M字节

/************************ 通用参数定义 ************************/
#define W25QX_PAGE_SIZE         256     // 每页字节数(全系列通用)
#define W25QX_SECTOR_SIZE       4096    // 每扇区字节数(全系列通用)

/************************ W25Qx 指令集 ************************/
#define W25QX_CMD_WREN          0x06    // 写使能
#define W25QX_CMD_WRDI          0x04    // 写禁用
#define W25QX_CMD_RDSR1         0x05    // 读状态寄存器1
#define W25QX_CMD_WRSR          0x01    // 写状态寄存器
#define W25QX_CMD_PP            0x02    // 页编程
#define W25QX_CMD_SE            0x20    // 扇区擦除(4K)
#define W25QX_CMD_BE32K         0x52    // 块擦除(32K)
#define W25QX_CMD_BE64K         0xD8    // 块擦除(64K)
#define W25QX_CMD_CE            0xC7    // 芯片擦除
#define W25QX_CMD_READ          0x03    // 普通读数据
#define W25QX_CMD_FAST_READ     0x0B    // 快速读数据
#define W25QX_CMD_RDID          0x90    // 读ID(2字节)
#define W25QX_CMD_JEDEC_ID      0x9F    // 读JEDEC ID(3字节)
#define W25QX_CMD_PWD           0xB9    // 掉电
#define W25QX_CMD_RELEASE_PWD   0xAB    // 释放掉电并读ID
#define W25QX_CMD_4BYTE_ADDR    0xB7    // 进入4字节地址模式
#define W25QX_CMD_EXIT_4BYTE    0xE9    // 退出4字节地址模式

/************************ 芯片型号枚举 ************************/
typedef enum {
    W25_Error = 0,
    W25Q80  = 0XEF13,
    W25Q16  = 0XEF14,
    W25Q32  = 0XEF15,
    W25Q64  = 0XEF16,
    W25Q128 = 0XEF17,  // 注意：W25Q128的ID是0XEF17，之前的0XEF18是旧版标识
    W25Q256 = 0XEF18
} W25Qx_TypeDef;

extern W25Qx_TypeDef W25Qx_CurrentType;// 当前检测到的芯片型号
extern uint32_t W25Qx_TotalSize;       // 当前芯片总容量(字节)
extern uint8_t W25Qx_4ByteAddrMode;    // 是否启用4字节地址模式(0:禁用 1:启用)

void GD25Qx_WakeUP(void);

uint8_t W25Qx_Init(void);
W25Qx_TypeDef W25Qx_ReadID(void);
uint8_t W25Qx_ReadStatusReg1(void);            // 读取状态寄存器1
void W25Qx_WriteEnable(void);                  // 写使能
void W25Qx_WriteDisable(void);                 // 写禁用
void W25Qx_WaitBusy(void);                     // 等待芯片空闲
void W25Qx_SectorErase(uint32_t addr);         // 扇区擦除(4K)
void W25Qx_PageProgram(uint32_t addr, uint8_t *pData, uint16_t len); // 页编程
void W25Qx_ReadData(uint32_t addr, uint8_t *pData, uint32_t len);    // 读数据
void W25Qx_ChipErase(void);                     // 芯片擦除(慎用)
void W25Qx_Test(void);

#endif



