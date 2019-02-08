#include "window.hpp"
#include "internal_anim.hpp"
#include "network.hpp"
#include "status.hpp"
#include "system_init.hpp"
#include "gpioes.h"


//Init internal peripheries
clock_and_systick  init_1;
gpio               init_2;
uart_and_dma       init_3;
hal_i2c            init_4;
hal_timer          init_5;

//Init external peripheries
windows::window windows::left_window(WINDOW_3V3_LEFT_GPIO_Port,	WINDOW_3V3_LEFT_Pin, WINDOW_POWER_LEFT_GPIO_Port, WINDOW_POWER_LEFT_Pin, USART2, DMA1, LL_DMA_CHANNEL_4 );
windows::window windows::right_window(WINDOW_3V3_RIGHT_GPIO_Port, WINDOW_3V3_RIGHT_Pin,	WINDOW_POWER_RIGHT_GPIO_Port, WINDOW_POWER_RIGHT_Pin, USART1, DMA1, LL_DMA_CHANNEL_2 );
net::network    net::inetwork;

extern "C" int main(void);

int main(void){
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
