#include <Arduino.h>

//w25q32 defination 
#define FLASH_BYTE      ((4) * 1024 * 1024)  //4MB
#define SECTOR_BYTE     ((4) * 1024) //4kb
#define BLOCK_BYTE      ((64) * 1024) //64kb
#define PAGE_BYTE       ((1) * 256) //256byte

#define PAGE_SIZE       (FLASH_BYTE / PAGE_BYTE)//((1) * 1024) //1024*256 = 4mb
#define SECTOR_SIZE     (FLASH_BYTE / SECTOR_BYTE)
#define BLOCK_SIZE      (FLASH_BYTE / BLOCK_BYTE)

#define FLASH_OK        0
#define FLASH_ERR       1
#define FLASH_ERR_PARAM 2


uint8_t init_flash(void);
uint8_t reset_flash(void);
void flash_set_cs(uint8_t cs);

uint16_t read_device_id(void);
void read_register(uint8_t cmd, uint8_t *regdata, uint8_t size);
uint8_t write_register(uint8_t cmd, uint8_t *regdata, uint8_t size);
uint8_t write_cmd(uint8_t cmd);
void write_enable(void);
void write_addr(uint32_t cmd, uint32_t address);
void wait_flash(void);

uint8_t erase_chip(void);
uint8_t erase_sector(uint16_t sector);

uint8_t write_flash(uint32_t address, uint8_t *data, uint32_t size);
uint8_t read_flash(uint32_t address, uint8_t *data, uint32_t size);

uint8_t enable_qpi(void);