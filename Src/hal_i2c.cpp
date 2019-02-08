#include "system_init.hpp"
#include "stm32f0xx_hal.h"
#include "error_handler.hpp"

extern "C" I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c2;

hal_i2c::hal_i2c(){
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x00404C74;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

        /**Configure Analogue filter 
        */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

        /**Configure Digital filter 
        */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
}
