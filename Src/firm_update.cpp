/*
 * firm_update.cpp
 *
 *  Created on: Dec 25, 2018
 *      Author: kisada
 */

#include <cstddef>
#include <cstdint>
#include "firm_update.hpp"


/*
 *  _______________________________________________________________________________________________________________
 *  |                                         |        |                                                 |        |
 *  |   Main firmware                         |  This  |     New firmware to be copied over the main     |  junk  |
 *  |                                         |  file  |                                                 |        |
 *  |_________________________________________|________|_________________________________________________|________| 
 *  0                                         31        32                                                63      64
 *  ^--- Flash page indexes
 *  
 */


namespace{
	const     uint8_t*  flash_begin = (uint8_t*)(0x8000000);
	constexpr size_t    page_size   = 1024; //1 kByte pre page

    uint64_t flash_checksum(const uint8_t* const from,  const uint8_t* const to){
        uint64_t res = 0;
        for(auto i = from; i != to; i++)
            res+=*i;
        return res;
    }
}

const uint64_t firmware_update::checksum_of_new_fw(){
    const uint8_t* from = flash_begin + (32*page_size);
    const uint8_t* to   = flash_begin + (63*page_size);
    
    return flash_checksum(from, to);
}


void firmware_update::refurbish /*[[noreturn]]*/ (){
    void* ptr  = const_cast<uint8_t*>(flash_begin + (31*page_size));
    void (*fgv)() = ptr;
}
