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
		
		unlock_flash(unlock_flash&) = delete;
        void operator=(unlock_flash&) = delete;
	};

    static constexpr size_t         pageSize   = 1024; // 1 KByte per page
	static constexpr uint8_t* const flash_addr = (uint8_t*)(0x8000000);

	void erasePage(const uint32_t page_num);
	
	void write_byte(const uint8_t towrite, volatile uint8_t* const addr); //A pointer a konstans nem az adat.

	void reprogramPage (const std::array<uint8_t, pageSize>& Buff, const uint32_t page_num);
}
