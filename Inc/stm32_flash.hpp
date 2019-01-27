#pragma once

#include <stddef.h>
#include <array>
#include "stm32f0xx.h"

namespace stm32_flash{
	//Reference: https://github.com/jgowans/stm32f0_devel/blob/master/STM32F0xx_StdPeriph_Lib_V1.3.1/Libraries/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_flash.c

	inline void WaitForLastOperation [[gnu::always_inline]] (){
		while ((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY);
		if((FLASH->SR & FLASH_FLAG_BSY) != FLASH_FLAG_EOP){
			while(1); //TODO better error handling
		}
	}

	struct unlock_flash{
		unlock_flash (){
			if((FLASH->CR & FLASH_CR_LOCK) != RESET){
				FLASH->KEYR = FLASH_KEY1;
				FLASH->KEYR = FLASH_KEY2;
			}
		}

		~unlock_flash (){
			FLASH->CR |= FLASH_CR_LOCK;
		}
	};

	static constexpr size_t         pageSize   = 1024; // 1 KByte per page
	static           uint8_t* const flash_addr = (uint8_t*)(0x8000000);

	void erasePage(const uint32_t page_num){
		WaitForLastOperation();

		FLASH->CR |= FLASH_CR_PER;
		FLASH->AR  = reinterpret_cast<uint32_t>(flash_addr + (pageSize * page_num)); //TODO investigate int narrowing warn
		FLASH->CR |= FLASH_CR_STRT;

		WaitForLastOperation();

		FLASH->CR &= ~FLASH_CR_PER;
	}

	
	void write_byte(const uint8_t towrite, volatile uint8_t* const addr){ //A pointer a konstans nem az adat.
		WaitForLastOperation();

		FLASH->CR |= FLASH_CR_PG;

		*addr = towrite;

		WaitForLastOperation();

		//Disable the PG Bit
		FLASH->CR &= ~FLASH_CR_PG;
	}

	void reprogramPage (const std::array<uint8_t, pageSize>& Buff, const uint32_t page_num){
		unlock_flash asd;

		erasePage(page_num);

		WaitForLastOperation();
		FLASH->CR |= FLASH_CR_PG;

		volatile uint8_t* ptr = flash_addr + (pageSize * page_num);
		for(const uint8_t& i : Buff){
			*ptr = i; //TODO look up if uint8_t write is possible over the uint16_t case
			 ptr++;

			WaitForLastOperation();
		}

		FLASH->CR &= ~FLASH_CR_PG;
	}
}
