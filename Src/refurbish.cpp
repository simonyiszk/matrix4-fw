/*
 * refurbish.cpp created on jan 25 '19 by Kiss Adam
 */

#include "stm32f0xx.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32_flash.hpp"

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
    inline void disable_interrupts(){
        const IRQn_Type interrupts_to_disable[] = { //See IRQn_Type definition in stm32f030x8.h before extend
            WWDG_IRQn                   ,// = 0,     /*!< Window WatchDog Interrupt                                       */
            RTC_IRQn                    ,//= 2,      /*!< RTC Interrupt through EXTI Lines 17, 19 and 20                  */
            FLASH_IRQn                  ,//= 3,      /*!< FLASH global Interrupt                                          */
            RCC_IRQn                    ,//= 4,      /*!< RCC global Interrupt                                            */
            EXTI0_1_IRQn                ,//= 5,      /*!< EXTI Line 0 and 1 Interrupt                                     */
            EXTI2_3_IRQn                ,//= 6,      /*!< EXTI Line 2 and 3 Interrupt                                     */
            EXTI4_15_IRQn               ,//= 7,      /*!< EXTI Line 4 to 15 Interrupt                                     */
            DMA1_Channel1_IRQn          ,//= 9,      /*!< DMA1 Channel 1 Interrupt                                        */
            DMA1_Channel2_3_IRQn        ,//= 10,     /*!< DMA1 Channel 2 and Channel 3 Interrupt                          */
            DMA1_Channel4_5_IRQn        ,//= 11,     /*!< DMA1 Channel 4 and Channel 5 Interrupt                          */
            ADC1_IRQn                   ,//= 12,     /*!< ADC1 Interrupt                                                  */
            TIM1_BRK_UP_TRG_COM_IRQn    ,//= 13,     /*!< TIM1 Break, Update, Trigger and Commutation Interrupt           */
            TIM1_CC_IRQn                ,//= 14,     /*!< TIM1 Capture Compare Interrupt                                  */
            TIM3_IRQn                   ,//= 16,     /*!< TIM3 global Interrupt                                           */
            TIM6_IRQn                   ,//= 17,     /*!< TIM6 global Interrupt                                           */
            TIM14_IRQn                  ,//= 19,     /*!< TIM14 global Interrupt                                          */
            TIM15_IRQn                  ,//= 20,     /*!< TIM15 global Interrupt                                          */
            TIM16_IRQn                  ,//= 21,     /*!< TIM16 global Interrupt                                          */
            TIM17_IRQn                  ,//= 22,     /*!< TIM17 global Interrupt                                          */
            I2C1_IRQn                   ,//= 23,     /*!< I2C1 Event Interrupt                                            */
            I2C2_IRQn                   ,//= 24,     /*!< I2C2 Event Interrupt                                            */
            SPI1_IRQn                   ,//= 25,     /*!< SPI1 global Interrupt                                           */
            SPI2_IRQn                   ,//= 26,     /*!< SPI2 global Interrupt                                           */
            USART1_IRQn                 ,//= 27,     /*!< USART1 global Interrupt                                         */
            USART2_IRQn                 //= 28       /*!< USART2 global Interrupt                                         */
        };
            
        for (const auto& i: interrupts_to_disable)
            NVIC_DisableIRQ(i);
        
        //Disable SysTick
        SysTick->CTRL  &= ! (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
    }
    
    inline void enable_hsi_oscillator(){
        LL_RCC_HSI_Enable();
        
        while(! LL_RCC_HSI_IsReady() );
    }
}

int main [[noreturn]] (){
	disable_interrupts();
	enable_hsi_oscillator();
	
	using namespace stm32_flash;

    unlock_flash raii;
    
    for(size_t page = 0; page <= 30; page++)
        erasePage(page);
    
    //No way back
    
    const size_t      bytes_to_copy = 31 * pageSize;
    volatile uint8_t* main_fw       = flash_addr;
    const uint8_t*    new_fw        = flash_addr + ( 32 * pageSize );
    
	for(size_t i = 0; i < bytes_to_copy; i++){
		write_byte(*new_fw, main_fw);
        
        main_fw++;
        new_fw ++;
	}

	NVIC_SystemReset(); //Amen
}
