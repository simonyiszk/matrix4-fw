#include "window.hpp"
#include "main2.hpp"
#include "internal_anim.hpp"
#include "network.hpp"
#include "status.hpp"

extern "C"{
#include "main.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_usart.h"
};


uint8_t emelet_szam =0;
uint8_t szoba_szam  =0;

void main2(void){
	//init_network(); reminder: static instances's constructors are ran before the driver initialization functions

	net::inetwork.init();

	windows::right_window.init();
	windows::left_window.init();

	status::turn_internal_anim_on();

	while (1){
		if(status::if_internal_animation_is_on)
			internal_animation::step_anim();

		net::inetwork.step_network();

		windows::left_window.step_state();
		windows::right_window.step_state();

		windows::left_window.update_image();
		windows::right_window.update_image();

		if(LL_GPIO_IsInputPinSet(USER_INPUT_BUTTON_GPIO_Port, USER_INPUT_BUTTON_Pin)){
			//TODO do sg
		}
	}

	//todo error handling
}
