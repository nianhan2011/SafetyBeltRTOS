#include "stm32f10x_flash.h"
#include "drv_flash.h"
#include "string.h"
#define StartServerManageFlashAddress ((u32)0x08036000) // 读写起始地址（内部flash的主存储块地址从0x08036000开始）
#define FLASH_SIZE 256                                  // 所选MCU的FLASH容量大小(单位为K)

#if FLASH_SIZE < 256
#define SECTOR_SIZE 1024 // 字节
#else
#define SECTOR_SIZE 2048 // 字节
#endif

// 从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress, uint16_t *writeData, uint16_t countToWrite)
{
    uint32_t offsetAddress = startAddress - FLASH_BASE;                      // 计算去掉0X08000000后的实际偏移地址
    uint32_t sectorPosition = offsetAddress / SECTOR_SIZE;                   // 计算扇区地址，对于STM32F103VET6为0~255
    uint32_t sectorStartAddress = sectorPosition * SECTOR_SIZE + FLASH_BASE; // 对应扇区的首地址
    uint16_t dataIndex;
    if (startAddress < FLASH_BASE || ((startAddress + countToWrite * 2) >= (FLASH_BASE + SECTOR_SIZE * FLASH_SIZE)))
    {
        return; // 非法地址
    }
    FLASH_Unlock(); // 解锁写保护

    FLASH_ErasePage(sectorStartAddress); // 擦除这个扇区

    for (dataIndex = 0; dataIndex < countToWrite; dataIndex++)
    {
        FLASH_ProgramHalfWord(startAddress + dataIndex * 2, writeData[dataIndex]);
    }

    FLASH_Lock(); // 上锁写保护
}

// 读取指定地址的半字(16位数据)
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
    return *(__IO uint16_t *)address;
}

// 从指定地址开始读取多个数据
void FLASH_ReadMoreData(uint32_t startAddress, uint16_t *readData, uint16_t countToRead)
{
    uint16_t dataIndex;
    for (dataIndex = 0; dataIndex < countToRead; dataIndex++)
    {
        readData[dataIndex] = FLASH_ReadHalfWord(startAddress + dataIndex * 2);
    }
}

void write_to_flash()
{
    FLASH_WriteMoreData(StartServerManageFlashAddress, (u16 *)&(drv_flash_pt->permissions_t), sizeof(Permissions_t) / 2);
}
void read_from_flash()
{
    memset(&(drv_flash_pt->permissions_t), 0, sizeof(Permissions_t));
    // u16 count_len = sizeof(ptr)  / 2;
    FLASH_ReadMoreData(StartServerManageFlashAddress, (u16 *)&(drv_flash_pt->permissions_t), sizeof(Permissions_t) / 2);
}

static Drv_Flash_t drv_flash_t = {
    .read_from_flash = read_from_flash,
    .write_to_flash = write_to_flash,
    .permissions_t = {
        .attitude_warn = 0,
        .emergency_unclock = 0,
        .height_check = 0,
        .height_auto_lock = 0,
    },
};

Drv_Flash_pt drv_flash_pt;
void init_flash(void)
{
    drv_flash_pt = &drv_flash_t;
}
