/*
 * firm_update.cpp
 *
 *  Created on: Dec 25, 2018
 *      Author: kisada
 */

#include <stddef.h>

namespace{
	constexpr uint8_t* flash_begin = reinterpret_cast<uint8_t*>(0x8000000);
	constexpr size_t   flash_size  = 64 * 1024;

	constexpr uint8_t* flash_half  = flash_begin + (flash_size / 2);
	constexpr uint8_t* flash_end   = flash_begin +  flash_size;
}

#include "firm_update.hpp"

using namespace firmware_update;

uint64_t flash_checksum(flashPart chunk){
	uint8_t* from = chunk == second ? flash_half : flash_begin; //TODO consider sum of 64 bit blocks
	uint8_t* to   = chnuk == first  ? flash_half : flash_end;

	uint64_t res = 0;
	for(auto i = from; i != to; i++)
		res+=*i;

	return res;
}

#include "stm32f0xx.h"

void copyfirmware [[noreturn]] (){
	//Turn on HSI

	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;


}

