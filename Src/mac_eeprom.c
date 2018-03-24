#include "mac_eeprom.h"
#include "stm32f0xx_hal.h"

//24AA02E48T-I/OT

extern I2C_HandleTypeDef hi2c2;

const uint16_t eeprom_address = 0b0000000010100000; // 0b1010000 with appropriate padding

const uint16_t eui48_start_addr = 0xFA;

void getMAC(uint8_t* dest){
	if (HAL_I2C_Mem_Read(&hi2c2, eeprom_address, eui48_start_addr, I2C_MEMADD_SIZE_8BIT, dest, 6, HAL_MAX_DELAY) != HAL_OK){ //TODO set max_dalay from the eeprom datasheet
		//Error handling
	}
}
