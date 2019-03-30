#pragma once

#include "stm32f0xx_ll_gpio.h"


/// Output

enum output_gpioes{
    WINDOW_POWER_LEFT,
    SPI1_NSS,
    W5500_RESET,
    LED_JOKER,
    LED_COMM,
    LED_DHCP,
    LED_HEARTH,
    WINDOW_POWER_RIGHT,
    WINDOW_3V3_RIGHT,
    WINDOW_3V3_LEFT
};

void reset_gpio(enum output_gpioes);  // ,,---- this is a high level interface for GPIO handle
#define reset_gpio(x) LL_GPIO_ResetOutputPin(x ## _GPIO_Port, x ## _Pin);

void set_gpio(enum output_gpioes);
#define set_gpio(x) LL_GPIO_SetOutputPin(x ## _GPIO_Port, x ## _Pin);

void toogle_gpio(enum output_gpioes);
#define toogle_gpio(x) LL_GPIO_TogglePin(x ## _GPIO_Port, x ## _Pin);

//These are the output of cubeMx, also the inputs of ST functions. Cannot throw them away :(

#define WINDOW_POWER_LEFT_Pin LL_GPIO_PIN_13
#define WINDOW_POWER_LEFT_GPIO_Port GPIOC
#define SPI1_NSS_Pin LL_GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define W5500_RESET_Pin LL_GPIO_PIN_1
#define W5500_RESET_GPIO_Port GPIOB
#define LED_JOKER_Pin LL_GPIO_PIN_12
#define LED_JOKER_GPIO_Port GPIOB
#define LED_COMM_Pin LL_GPIO_PIN_13
#define LED_COMM_GPIO_Port GPIOB
#define LED_DHCP_Pin LL_GPIO_PIN_14
#define LED_DHCP_GPIO_Port GPIOB
#define LED_HEARTH_Pin LL_GPIO_PIN_15
#define LED_HEARTH_GPIO_Port GPIOB
#define WINDOW_POWER_RIGHT_Pin LL_GPIO_PIN_11
#define WINDOW_POWER_RIGHT_GPIO_Port GPIOA
#define WINDOW_3V3_RIGHT_Pin LL_GPIO_PIN_8
#define WINDOW_3V3_RIGHT_GPIO_Port GPIOB
#define WINDOW_3V3_LEFT_Pin LL_GPIO_PIN_9
#define WINDOW_3V3_LEFT_GPIO_Port GPIOB

#define WINDOW_I2C_SCL_RIGHT_GPIO_Port GPIOA
#define WINDOW_I2C_SCL_RIGHT_Pin LL_GPIO_PIN_9
#define WINDOW_I2C_SDA_RIGHT_GPIO_Port GPIOA
#define WINDOW_I2C_SDA_RIGHT_Pin LL_GPIO_PIN_10
#define WINDOW_I2C_SCL_LEFT_GPIO_Port GPIOA
#define WINDOW_I2C_SCL_LEFT_Pin LL_GPIO_PIN_2
#define WINDOW_I2C_SDA_LEFT_GPIO_Port GPIOA
#define WINDOW_I2C_SDA_LEFT_Pin LL_GPIO_PIN_3

/// Input

enum input_gpioes{
    USER_INPUT_BUTTON
};

uint32_t read_gpio(enum input_gpioes);
#define read_gpio(x) LL_GPIO_IsInputPinSet(x ## _GPIO_Port, x ## _Pin);

#define USER_INPUT_BUTTON_Pin LL_GPIO_PIN_2
#define USER_INPUT_BUTTON_GPIO_Port GPIOB
