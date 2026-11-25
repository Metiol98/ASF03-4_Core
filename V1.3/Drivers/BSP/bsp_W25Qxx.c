#include "bsp_W25Qxx.h"

W25Qx_TypeDef W25Qx_CurrentType;
uint32_t W25Qx_TotalSize;
uint8_t W25Qx_4ByteAddrMode;

/***************** 硬件驱动接口重新封装 --START *************/
/**
 * @brief  Flash 发送一个字节并接收一个字节
 * @param  byte: 要发送的字节
 * @retval 接收到的字节
 */
static uint8_t SPI_Flash_SendByte(uint8_t byte)
{
  uint8_t recv_byte = 0;
  HAL_SPI_TransmitReceive(W25QX_SPI_HANDLE,&byte,&recv_byte,1,10);
	return recv_byte;
}


/**
 * @brief  Flash接收多个字节
 * @param  pData: 接收数据缓冲区
 * @param  len: 接收长度
 */
static void SPI_Flash_ReceiveBytes(uint8_t *pData, uint16_t len)
{
    // 发送空字节来接收数据
    uint8_t dummy = 0xFF;
    for(uint16_t i=0; i<len; i++)
    {
        pData[i] = SPI_Flash_SendByte(dummy);
    }
}

/**
 * @brief  Flash WP引脚被UART占用重新配置
 */
static void SPI_Flash_ReconfigToGpioOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// 关闭 UART 防止引脚冲突
  __HAL_UART_DISABLE(&huart3);
	
	GPIO_InitStruct.Pin 	= Flash_WP_Pin;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(Flash_WP_GPIO_Port,&GPIO_InitStruct);
	
	// 重新开启 UART（只影响 RX，TX 已经脱离 UART 控制）
  __HAL_UART_ENABLE(&huart3);
	
	// 默认输出高电平（你可以自己改）
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
}
/***************** 硬件驱动接口重新封装--END *************/

/**
 * @brief  W25Qx初始化
 * @retval 0:成功 1:芯片检测失败
 */
uint8_t W25Qx_Init(void)
{
	SPI_Flash_ReconfigToGpioOutput();
	
	W25QX_CS_HIGH();		// 未选中芯片
  W25QX_WP_HIGH();    // 禁用写保护
  W25QX_HOLD_HIGH();  // 取消保持
	
	GD25Qx_WakeUP();
	
	//检测芯片
	W25Qx_CurrentType = W25Qx_ReadID();
	if(W25Qx_CurrentType == 0)
		return 1; // 芯片检测失败

	// 8. 设置芯片容量和地址模式
	switch(W25Qx_CurrentType)
	{
			case W25Q80:    W25Qx_TotalSize = 1*1024*1024;  break; // 1MB
			case W25Q16:    W25Qx_TotalSize = 2*1024*1024;  break; // 2MB
			case W25Q32:    W25Qx_TotalSize = 4*1024*1024;  break; // 4MB
			case W25Q64:    W25Qx_TotalSize = 8*1024*1024;  break; // 8MB
			case W25Q128:   W25Qx_TotalSize = 16*1024*1024; break; // 16MB
			case W25Q256:   W25Qx_TotalSize = 32*1024*1024; // 32MB
							W25Qx_4ByteAddrMode = 1;       // W25Q256需要4字节地址
															break;
			default:        return 1;
	}

	return 0;			// 初始化成功
}

/**
 * @brief  读取W25Qx芯片ID
 * @retval 16位ID: W25Q128固定为0xEF17
 */
W25Qx_TypeDef W25Qx_ReadID(void)
{
	uint16_t chip_id = 0;

	W25QX_CS_LOW();
	SPI_Flash_SendByte(W25QX_CMD_JEDEC_ID);   // 发送读ID指令
	uint8_t mid = SPI_Flash_SendByte(0xFF);		//厂商ID
	uint8_t id1 = SPI_Flash_SendByte(0xFF);		//类型
	uint8_t id2 = SPI_Flash_SendByte(0xFF);		//容量
	W25QX_CS_HIGH();

	
	if(mid == 0xEF)
	{
		chip_id = (mid << 8) | id2;
	}
	else if(mid == 0xC8)
	{
		chip_id = (mid << 8) | id2;
	}
	else
	{
		return 0;
	}
	
	// 识别芯片型号
	switch(chip_id)
	{
		case 0XEF13: return W25Q80;
		case 0XEF14: return W25Q16;
		case 0XEF15: return W25Q32;
		case 0XEF16: return W25Q64;
		case 0XEF17: return W25Q128;
		case 0XEF18: return W25Q256;
		
		case 0XC814: return W25Q80;   // GD25Q80
		case 0XC815: return W25Q16;   // GD25Q16
		case 0XC816: return W25Q32;   // GD25Q32
		case 0XC817: return W25Q64;   // GD25Q64
		case 0XC818: return W25Q128;  // GD25Q128
		case 0XC819: return W25Q256;  // GD25Q256
		default:     return 0; // 未知芯片
	}
}

/**
 * @brief  GD25Qxx唤醒（W25Q上电默认正常工作模式,GD上电默认保守状态需要先唤醒）
 * @retval None
 */
void GD25Qx_WakeUP(void)
{
	W25QX_CS_LOW();
	SPI_Flash_SendByte(W25QX_CMD_RELEASE_PWD);   //唤醒
	W25QX_CS_HIGH();
}

/**
 * @brief  写使能(擦除/编程前必须调用)
 * @retval None
 */
void W25Qx_WriteEnable(void)
{
	W25QX_CS_LOW();
	SPI_Flash_SendByte(W25QX_CMD_WREN);
	W25QX_CS_HIGH();
}

/**
 * @brief  写禁止(擦除/编程后必须调用)
 * @retval None
 */
void W25Qx_WriteDisable(void)
{
	W25QX_CS_LOW();
	SPI_Flash_SendByte(W25QX_CMD_WRDI);
	W25QX_CS_HIGH();
}

/**
 * @brief  读取状态寄存器1
 * @retval 状态寄存器值
 */
static uint8_t W25Qx_ReadStatusReg1(void)
{
	uint8_t reg_val = 0;

	W25QX_CS_LOW();
	SPI_Flash_SendByte(W25QX_CMD_RDSR1);
	reg_val = SPI_Flash_SendByte(0xFF);
	W25QX_CS_HIGH();

	return reg_val;
}

/**
 * @brief  等待W25Q128空闲(BUSY位为0)
 * @retval None
 */
void W25Qx_WaitBusy(void)
{
	while((W25Qx_ReadStatusReg1() & 0x01) == 0x01);
}

/**
 * @brief  W25Q128扇区擦除(4K字节)
 * @param  addr: 扇区起始地址（需对齐4K，如0x000000、0x001000）
 * @note   擦除时间约30~100ms，擦除后数据为0xFF
 */
void W25Qx_SectorErase(uint32_t addr)
{
	W25Qx_WriteEnable();
	W25Qx_WaitBusy();

	W25QX_CS_LOW();
	if(W25Qx_4ByteAddrMode)
	{
		SPI_Flash_SendByte(W25QX_CMD_4BYTE_ADDR); // 4字节地址模式
		SPI_Flash_SendByte(W25QX_CMD_SE);
		SPI_Flash_SendByte((addr >> 24) & 0xFF);
		SPI_Flash_SendByte((addr >> 16) & 0xFF);
		SPI_Flash_SendByte((addr >> 8) & 0xFF);
		SPI_Flash_SendByte(addr & 0xFF);
	}
	else
	{
		SPI_Flash_SendByte(W25QX_CMD_SE);
		SPI_Flash_SendByte((addr >> 16) & 0xFF);
		SPI_Flash_SendByte((addr >> 8) & 0xFF);
		SPI_Flash_SendByte(addr & 0xFF);
	}
	W25QX_CS_HIGH();

	W25Qx_WaitBusy();
}

/**
 * @brief  页编程(单次最多256字节)
 * @param  addr: 写入起始地址
 * @param  pData: 写入数据缓冲区
 * @param  len: 写入长度
 */
void W25Qx_PageProgram(uint32_t addr, uint8_t *pData, uint16_t len)
{
	if(len == 0 || pData == NULL) return;
	if(len > W25QX_PAGE_SIZE) len = W25QX_PAGE_SIZE;

	W25Qx_WriteEnable();
	W25Qx_WaitBusy();

	W25QX_CS_LOW();
	if(W25Qx_4ByteAddrMode)
	{
		SPI_Flash_SendByte(W25QX_CMD_4BYTE_ADDR);
		SPI_Flash_SendByte(W25QX_CMD_PP);
		SPI_Flash_SendByte((addr >> 24) & 0xFF);
		SPI_Flash_SendByte((addr >> 16) & 0xFF);
		SPI_Flash_SendByte((addr >> 8) & 0xFF);
		SPI_Flash_SendByte(addr & 0xFF);
	}
	else
	{
		SPI_Flash_SendByte(W25QX_CMD_PP);
		SPI_Flash_SendByte((addr >> 16) & 0xFF);
		SPI_Flash_SendByte((addr >> 8) & 0xFF);
		SPI_Flash_SendByte(addr & 0xFF);
	}

	for(uint8_t i=0;i<len;i++)
		SPI_Flash_SendByte(pData[i]);
	
	W25QX_CS_HIGH();

	W25Qx_WaitBusy();
}

/**
 * @brief  读取数据(可连续读取任意长度)
 * @param  addr: 读取起始地址
 * @param  pData: 接收数据缓冲区
 * @param  len: 读取长度
 */
void W25Qx_ReadData(uint32_t addr, uint8_t *pData, uint32_t len)
{
	if(len == 0 || pData == NULL) return;

	W25QX_CS_LOW();
	if(W25Qx_4ByteAddrMode)
	{
		SPI_Flash_SendByte(W25QX_CMD_4BYTE_ADDR);
		SPI_Flash_SendByte(W25QX_CMD_READ);
		SPI_Flash_SendByte((addr >> 24) & 0xFF);
		SPI_Flash_SendByte((addr >> 16) & 0xFF);
		SPI_Flash_SendByte((addr >> 8) & 0xFF);
		SPI_Flash_SendByte(addr & 0xFF);
	}
	else
	{
		SPI_Flash_SendByte(W25QX_CMD_READ);
		SPI_Flash_SendByte((addr >> 16) & 0xFF);
		SPI_Flash_SendByte((addr >> 8) & 0xFF);
		SPI_Flash_SendByte(addr & 0xFF);
	}

	SPI_Flash_ReceiveBytes(pData, len);
	W25QX_CS_HIGH();
}

/**
 * @brief  芯片擦除(慎用，耗时极长)
 */
void W25Qx_ChipErase(void)
{
	W25Qx_WriteEnable();
	W25Qx_WaitBusy();

	W25QX_CS_LOW();
	SPI_Flash_SendByte(W25QX_CMD_CE);
	W25QX_CS_HIGH();

	W25Qx_WaitBusy(); // W25Q256擦除需几分钟，W25Q128需几十秒
}

/**
 * @brief  W25Qx测试函数
 */
void W25Qx_Test(void)
{
	uint8_t write_buf1[32] = "Hello W25Qx HAL!";
  uint8_t read_buf1[32] = {0};
	W25Qx_SectorErase(0x000000);
	W25Qx_PageProgram(0x000000, write_buf1, sizeof(write_buf1));
	W25Qx_ReadData(0x000000, read_buf1, sizeof(read_buf1));
	//验证数据(示例)
	if(memcmp(write_buf1, read_buf1, sizeof(write_buf1)) == 0)
	{
		// 数据读写成功
	}
}
