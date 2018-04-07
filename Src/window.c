#include "window.h"
#include "stm32f0xx_hal.h"
//#include "mxconstants.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


enum twindow_status window_status[2] = {discharge_caps, discharge_caps};

uint8_t sec_cntr_window = 0;

struct pixel_data pixels[2][num_of_pixels];

void init_window_state_machine(){
	//GPIO default states configured along with the GPIO itself
	/*HAL_GPIO_SET(...
	 * turn off 3.3V
	 * turn off 12V
	 * configure UART
	 */

	//gloab vars are set from .bss section, no need for further init
	/*window_status[0]=vcc_3v3_off;
	window_status[1]=vcc_3v3_off;*/

	//gloab vars are set from .bss section, no need for further init
	/*frame_status[0]=buffer_free;
	frame_status[1]=buffer_free;*/

	//gloab vars are set from .bss section, no need for further init
	//sec_cntr_window = 0;

	for(size_t i=0; i<2;i++)
		for(size_t j=0; j<num_of_pixels; j++){
			pixels[i][j].blue=0;
			pixels[i][j].red=0;
			pixels[i][j].green=0;
			pixels[i][j].stat=buffer_free;
		}

	//TODO> DMA
}

uint8_t check_uart_welcome_message(enum window_selector w){
	//todo
	return 1;
}

void step_window_state(enum window_selector w){
	switch(window_status[(size_t)w]){
		case vcc_12v_on:
			for(size_t j=0; j<num_of_pixels; j++){
				if(pixels[(size_t) w][j].stat == buffer_full){
					pixels[(size_t) w][j].stat=buffer_free; //todo reconsider command order
					set_pixel(w, j, pixels[(size_t) w][j].red, pixels[(size_t) w][j].green, pixels[(size_t) w][j].blue);
					return;
				}
			}
		case vcc_12v_off:
			break;
		case vcc_3v3_off:
			if(sec_cntr_window>4)
				set_window_state(w, vcc_3v3_on);
			break;
		case vcc_3v3_on:
			if(sec_cntr_window>1){
				if(check_uart_welcome_message(w))
					set_window_state(w, vcc_12v_on);
				else
					set_window_state(w, vcc_3v3_off);
			}
			break;
		case discharge_caps:
			if(sec_cntr_window>10)
				set_window_state(w, vcc_3v3_off);
			break;
		default:
			set_window_state(w, discharge_caps);
	}
}

enum twindow_status get_window_state(enum window_selector w){
	return window_status[(size_t)w];
}

void set_window_state(enum window_selector w , enum twindow_status s){
	switch(s){
	default:
		s = discharge_caps;
	case discharge_caps:
		sec_cntr_window=0;
		if(w == left_window)
			HAL_GPIO_WritePin(WINDOW_3V3_LEFT_GPIO_Port, WINDOW_3V3_LEFT_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(WINDOW_3V3_RIGHT_GPIO_Port, WINDOW_3V3_RIGHT_Pin, GPIO_PIN_SET);
		break;
	case vcc_3v3_off:
		sec_cntr_window=0;
		if(w == left_window){
			HAL_GPIO_WritePin(WINDOW_3V3_LEFT_GPIO_Port, WINDOW_3V3_LEFT_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, GPIO_PIN_RESET);
		} else{
			HAL_GPIO_WritePin(WINDOW_3V3_RIGHT_GPIO_Port, WINDOW_3V3_RIGHT_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, GPIO_PIN_RESET);
		}
		break;
	case vcc_3v3_on:
		sec_cntr_window=0;
		//EMPTY DMA BUFFER
		if(w == left_window){
					HAL_GPIO_WritePin(WINDOW_3V3_LEFT_GPIO_Port, WINDOW_3V3_LEFT_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, GPIO_PIN_RESET);
				} else{
					HAL_GPIO_WritePin(WINDOW_3V3_RIGHT_GPIO_Port, WINDOW_3V3_RIGHT_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, GPIO_PIN_RESET);
				}
		break;
	case vcc_12v_on:
		if(w == left_window){
			HAL_GPIO_WritePin(WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, GPIO_PIN_SET);
		} else{
			HAL_GPIO_WritePin(WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, GPIO_PIN_SET);
		}
		break;
	case vcc_12v_off:
		if(w == left_window){
			HAL_GPIO_WritePin(WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, GPIO_PIN_RESET);
		} else{
			HAL_GPIO_WritePin(WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, GPIO_PIN_RESET);
		}
		break;
	}
	window_status[(size_t) w] = s;
}

void uartsend(enum window_selector w, char c){
	uint8_t buff[1];
	buff[0]=c;
	if(w == left_window){
		HAL_UART_Transmit(&huart2, buff, 1, HAL_MAX_DELAY);
	} else {
		HAL_UART_Transmit(&huart1, buff, 1, HAL_MAX_DELAY);
	}
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
