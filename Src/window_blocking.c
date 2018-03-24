#include "window_blocking.h"
#include "stm32f0xx_hal.h"
//#include "mxconstants.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

char if_window(enum window_selector w){
	if(w==left_window){
		return HAL_GPIO_ReadPin(WINDOW_LEFT_INSTALLED_GPIO_Port, WINDOW_LEFT_INSTALLED_Pin);
	}else {
		return HAL_GPIO_ReadPin(WINDOW_RIGHT_INSTALLED_GPIO_Port, WINDOW_RIGHT_INSTALLED_Pin);
	}
}

void turn_12V_on(enum window_selector w){
	if(w == left_window){
		HAL_GPIO_WritePin(WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, GPIO_PIN_SET);
	} else{
		HAL_GPIO_WritePin(WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, GPIO_PIN_SET);
	}
}

void turn_12V_off(enum window_selector w){
	if(w == left_window){
		HAL_GPIO_WritePin(WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, GPIO_PIN_RESET);
	} else{
		HAL_GPIO_WritePin(WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, GPIO_PIN_RESET);
	}
}

void uartsend(enum window_selector w, char c){
	uint8_t buff[1];
	buff[0]=c;
	volatile char ret = 0;
	if(w == left_window){
		ret = HAL_UART_Transmit(&huart2, buff, 1, HAL_MAX_DELAY);
	} else {
		ret = HAL_UART_Transmit(&huart1, buff, 1, HAL_MAX_DELAY);
	}
	ret += 1;
}

void set_pixel(enum window_selector w, unsigned char pixel, unsigned char red, unsigned char green, unsigned char blue){
	uint8_t pixel_inf;

	pixel &= 0x03;
	red   &= 0x0F;
	green &= 0x0F;
	blue  &= 0x0F;

	pixel_inf = pixel*3;

	uartsend(w, 0xF0);
	uartsend(w, (pixel_inf)<<4   | red);
	uartsend(w, (pixel_inf+1)<<4 | green);
	uartsend(w, (pixel_inf+2)<<4 | blue);
	uartsend(w, 0xF1);
	uartsend(w, (pixel_inf)<<4   | red);
	uartsend(w, (pixel_inf+1)<<4 | green);
	uartsend(w, (pixel_inf+2)<<4 | blue);
}
