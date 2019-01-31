#include "stm32_flash.hpp"

using namespace stm32_flash;
using stm32_flash::pageSize;

void stm32_flash::reprogramPage (const std::array<uint8_t, pageSize>& Buff, const uint32_t page_num) {
    unlock_flash asd;

    erasePage(page_num);

    WaitForLastOperation();
    FLASH->CR |= FLASH_CR_PG;

    volatile uint8_t* ptr = flash_addr + (pageSize * page_num);
    for(const uint8_t& i : Buff) {
        *ptr = i; //TODO look up if uint8_t write is possible over the uint16_t case
        ptr++;

        WaitForLastOperation();
    }

    FLASH->CR &= ~FLASH_CR_PG;
}

void stm32_flash::write_byte(const uint8_t towrite, volatile uint8_t* const addr) { //A pointer a konstans nem az adat.
    WaitForLastOperation();

    FLASH->CR |= FLASH_CR_PG;

    *addr = towrite;

    WaitForLastOperation();

    //Disable the PG Bit
    FLASH->CR &= ~FLASH_CR_PG;
}

void stm32_flash::erasePage(const uint32_t page_num) {
    WaitForLastOperation();

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR  = reinterpret_cast<uint32_t>(flash_addr + (pageSize * page_num)); //TODO investigate int narrowing warn
    FLASH->CR |= FLASH_CR_STRT;

    WaitForLastOperation();

    FLASH->CR &= ~FLASH_CR_PER;
}
