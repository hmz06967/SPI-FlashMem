#include <SPI.h>
#include "spi_flash.h"

uint8_t FLASH_CS = 5;

void flash_set_cs(uint8_t cs){
  FLASH_CS = cs;
}

uint8_t init_flash(void){
  uint8_t status = FLASH_OK;
  uint8_t wr[1] = {0x00};
  // put your setup code here, to run once:
  pinMode(FLASH_CS, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  //write_enable();
  digitalWrite(FLASH_CS, HIGH);
  SPI.transfer(0XAB); // POWER komutunu gönder
  read_device_id();
  status = write_register(0x01, wr, 1);//KORUMAYI kaldır
  wait_flash();
  return status;
}

void read_register(uint8_t cmd, uint8_t *regdata, uint8_t size){
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(cmd); // JEDEC ID komutunu gönder
  for (uint8_t i=0; i<size; i++) {
    regdata[i] = SPI.transfer(0x00);
  }
  digitalWrite(FLASH_CS, HIGH);
}

uint8_t write_register(uint8_t cmd, uint8_t *regdata, uint8_t size){
  uint8_t status = FLASH_OK;
  wait_flash();
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(cmd); // WRITE ID komutunu gönder
  for (uint8_t i=0; i<size; i++) {
    status = SPI.transfer(regdata[i]);
  }
  digitalWrite(FLASH_CS, HIGH);
  return status;
}

uint8_t write_cmd(uint8_t cmd){
  uint8_t status = FLASH_OK;
  digitalWrite(FLASH_CS, LOW);
  status = SPI.transfer(cmd); // RESUME komutunu gönder
  digitalWrite(FLASH_CS, HIGH);
  return status==FLASH_OK;
}

void write_addr(uint32_t cmd, uint32_t address){
  SPI.transfer(cmd); 
  SPI.transfer((address >> 16) & 0xff); //(MSB)
  SPI.transfer((address >> 8) & 0xff); 
  SPI.transfer(address & 0xff); // (LSB)
}

void wait_flash(){
  uint8_t reg1[1], is_busy = 1;
  while(is_busy){
    read_register(0x05, reg1, 1);
    is_busy = ((reg1[0]) & 0x01);
    delay(1);
  }
}

void write_enable(){
  write_cmd(0x06);
  //Wait
  uint8_t reg1[1], is_enable = 0;
  while(!is_enable){
    read_register(0x05, reg1, 1);
    is_enable = ((reg1[0]) & 0x02);
    delay(1);
  }
}

uint8_t erase_chip(){
  uint8_t status = 1;
  uint8_t reg1[1];
  //enable
  write_enable();
  //erase
  status = write_cmd(0xC7);
  //waiting
  wait_flash();
  return status==FLASH_OK;
}

uint8_t erase_sector(uint16_t sector){
  if(sector>SECTOR_SIZE){
    return FLASH_ERR_PARAM;
  }
  //enable
  write_enable();
  //resume
  write_cmd(0x7A);
  //erase sector
  write_cmd(0x20);
  //waiting finish
  wait_flash();
  return FLASH_OK;
}

uint8_t write_flash(uint32_t address, uint8_t *data, uint32_t size){ 
  uint8_t status = FLASH_OK;
  int32_t page_byte = size;

  if(size > (PAGE_SIZE * PAGE_BYTE)){
    return FLASH_ERR_PARAM;
  }
  //write map
  while(page_byte > 0){
    //enable
    write_enable();

    size = ((page_byte>PAGE_BYTE) ? PAGE_BYTE : page_byte);
    //write page
    digitalWrite(FLASH_CS, LOW);
    write_addr(0x02, address);
    //write data

    for (uint8_t i=0; i<(size); i++) {
      status = SPI.transfer(data[i]);
    }
    digitalWrite(FLASH_CS, HIGH);

    address += PAGE_BYTE;
    page_byte -=PAGE_BYTE;
    
    //waiting finish
    //wait_flash();
  }

  return FLASH_OK;
}

uint8_t read_flash(uint32_t address, uint8_t *data, uint32_t size){ 
  uint8_t status = FLASH_OK;
  digitalWrite(FLASH_CS, LOW);
  write_addr(0x03, address);
  for (uint32_t i=0; i<size; i++) {
    data[i] = SPI.transfer(0x00);
  }
  digitalWrite(FLASH_CS, HIGH);
  return status;
}

uint16_t read_device_id(){
  uint8_t read_data[3];
  read_register(0x9F, read_data, 3);

  return read_data[0]<<8 | read_data[1];
}

uint8_t reset_flash(){
  uint8_t status = FLASH_OK;
  write_cmd(0x66);
  wait_flash();
  status = write_cmd(0x99);
  wait_flash();
  return status == FLASH_OK;
}

uint8_t enable_qpi(){
  uint8_t status = FLASH_OK;
  status = write_cmd(0x38);
  wait_flash();
  return status == FLASH_OK;
}