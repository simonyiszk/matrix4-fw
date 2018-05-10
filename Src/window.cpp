#include "window.hpp"
#include "window.h"

using namespace windows;

/*****************************
 *    Class pixel_data
 *****************************/

pixel_data::pixel_data() :
		red(0),
		green(0),
		blue(0),
		stat(pixel_data::buffer_full)
{

}

void pixel_data::set(unsigned char red, unsigned char green, unsigned char blue){
	this->red=red;
	this->green=green;
	this->blue=blue;
	this->stat=pixel_data::buffer_full;
}

void pixel_data::flush(){
	this->stat=pixel_data::buffer_free;
}

bool pixel_data::isFull(){
	return this->stat == pixel_data::buffer_full;
}

/*****************************
 *    Class window
 *****************************/

void window::step_state(){
	switch(this->status){
		case vcc_12v_on:
			this->update_image();
			break;
		case vcc_12v_off:
			break;
		case vcc_3v3_off:
			if(sec_cntr_window>4)
				this->set_state(vcc_3v3_on);
			break;
		case vcc_3v3_on:
			if(sec_cntr_window>1){
				if(check_uart_welcome_message())
					this->set_state(vcc_12v_on);
				else
					this->set_state(vcc_3v3_off);
			}
			break;
		case discharge_caps:
			if(sec_cntr_window>10)
				this->set_state(vcc_3v3_off);
			break;
		default:
			this->set_state(discharge_caps);
	}
}

window::window(GPIO_TypeDef* gpio_port_3v3, uint16_t gpio_pin_3v3, GPIO_TypeDef* gpio_port_power, uint16_t gpio_pin_power, UART_HandleTypeDef* uart_handler):
		status(vcc_3v3_off),
		gpio_port_3v3(gpio_port_3v3),
		gpio_port_power(gpio_port_power),
		gpio_pin_3v3(gpio_pin_3v3),
		gpio_pin_power(gpio_pin_power),
		uart_handler(uart_handler)
{
	//this->set_state(discharge_caps); WARN static insatance's constructor runs before hardware init routines
	//TODO> DMA
};

window::twindow_status window::get_state(){
	return this->status;
}

void window::set_state(enum twindow_status new_stat){
	switch(new_stat){
		default:
			new_stat = discharge_caps;
		case discharge_caps:
			sec_cntr_window=0;
			HAL_GPIO_WritePin(gpio_port_3v3, gpio_pin_3v3, GPIO_PIN_SET);
			break;
		case vcc_3v3_off:
			sec_cntr_window=0;
			HAL_GPIO_WritePin(gpio_port_3v3, gpio_pin_3v3, GPIO_PIN_SET);
			HAL_GPIO_WritePin(gpio_port_power, gpio_pin_power, GPIO_PIN_RESET);
			break;
		case vcc_3v3_on:
			sec_cntr_window=0;
			//EMPTY DMA BUFFER
			HAL_GPIO_WritePin(gpio_port_3v3, gpio_pin_3v3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(gpio_port_power, gpio_pin_power, GPIO_PIN_RESET);
			break;
		case vcc_12v_on:
			HAL_GPIO_WritePin(gpio_port_power, gpio_pin_power, GPIO_PIN_SET);
			break;
		case vcc_12v_off:
			HAL_GPIO_WritePin(gpio_port_power, gpio_pin_power, GPIO_PIN_RESET);
			break;
		}
		this->status = new_stat;
}

bool window::check_uart_welcome_message(){
	//TODO DMA things
	return true;
}

void window::update_image(){
	for(size_t j=0; j<num_of_pixels; j++){
		if(pixels[j].isFull()){
			pixels[j].flush();
			uint8_t base = (j & 3) * 3;
			uint8_t buff[] = {
					0xF0,
					(uint8_t)( ((base + 0) << 4) | (uint8_t)(pixels[j].red   & (uint8_t)0x0F) ),
					(uint8_t)( ((base + 1) << 4) | (uint8_t)(pixels[j].green & (uint8_t)0x0F) ),
					(uint8_t)( ((base + 2) << 4) | (uint8_t)(pixels[j].blue  & (uint8_t)0x0F) )
			};
			HAL_UART_Transmit(uart_handler, buff, sizeof(buff), HAL_MAX_DELAY);
			return;
		}
	}
}

void window::blank(){
	for(size_t j=0; j<num_of_pixels; j++)
		pixels[j].set(0,0,0);
}


/*****************************
 *  Instance static classes
 *****************************/

windows::window windows::left_window(WINDOW_3V3_LEFT_GPIO_Port, WINDOW_3V3_LEFT_Pin, WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, &huart1);
windows::window windows::right_window(WINDOW_3V3_RIGHT_GPIO_Port, WINDOW_3V3_RIGHT_Pin, WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, &huart2);

uint8_t sec_cntr_window = 0;

