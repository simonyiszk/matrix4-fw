/*
 * firm_update.cpp
 *
 *  Created on: Dec 25, 2018
 *      Author: kisada
 */

#include <cstddef>
#include <cstdint>

namespace{
	/*constexpr*/ uint8_t* flash_begin = reinterpret_cast<uint8_t*>(0x8000000);
	/*constexpr*/ size_t   page_size   = 1024; //1 kByte pre page //TODO fix constexpr
	constexpr size_t       num_of_pages= 64;
	/*constexpr*/ size_t   flash_size  = num_of_pages * page_size;

	/*constexpr*/ uint8_t* flash_half  = flash_begin + (flash_size / 2);
	/*constexpr*/ uint8_t* flash_end   = flash_begin +  flash_size;
}

#include "firm_update.hpp"

using namespace firmware_update;

uint64_t flash_checksum(const uint8_t* const from,  const uint8_t* const to){
	uint64_t res = 0;
	for(auto i = from; i != to; i++)
		res+=*i;

	return res;
}

#include "stm32f0xx.h"
#include "stm32_flash.hpp"

static void really_copyfirmware [[noreturn, gnu::noinline, gnu::section("fw_updater")]] (){
	using namespace stm32_flash;

	for(size_t page = 0; page < num_of_pages/2-1; page++){
		const uint8_t* from = flash_half + ( page_size * page );

		reprogramPage(from, reinterpret_cast<std::array<unsigned char, 1024>>(page));
	}
}
