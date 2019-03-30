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
network    inetwork;

extern "C" int main(void);
bool init_i2c = false;

extern "C" TIM_HandleTypeDef htim16;
TIM_TypeDef* timer_asd = TIM16;

int main(void){
	status::turn_internal_anim_on();
	windows::left_window.init_I2C(0x60, WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin, WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin, TIM16, 20000000, 100000);
	windows::right_window.init_I2C(0x60, WINDOW_I2C_SCL_RIGHT_GPIO_Port, WINDOW_I2C_SCL_RIGHT_Pin, WINDOW_I2C_SDA_RIGHT_GPIO_Port, WINDOW_I2C_SDA_RIGHT_Pin, TIM16, 20000000, 100000);
//	//windows::left_window.init_I2C(0x61, WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin, WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin, TIM16, 20000000, 100000);
//	//windows::right_window.init_leds();
//	//volatile uint32_t t = this->timer->CNT;
	LL_GPIO_SetOutputPin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);
	for (int i = 0; i < 2000; ++i) {
		volatile uint32_t t = timer_asd->CNT;
		while((timer_asd->CNT - t) < 10000);
	}
//
	LL_GPIO_ResetOutputPin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);
	while (1){
		if(status::if_internal_animation_is_on)
			internal_animation::step_anim();

		inetwork.step_network();

		windows::right_window.step_state();
		windows::left_window.step_state();

		//windows::left_window.update_image();
		//windows::right_window.update_image();

		if(!LL_GPIO_IsInputPinSet(USER_INPUT_BUTTON_GPIO_Port, USER_INPUT_BUTTON_Pin)){
			//LL_GPIO_SetOutputPin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);
			if (!init_i2c)
			{
				windows::left_window.init_leds();
				for (int i = 0; i < 2000; ++i) {
					volatile uint32_t t = timer_asd->CNT;
					while((timer_asd->CNT - t) < 10000);
				}
				windows::left_window.pixels[3].set(30, 50, 255);
				windows::left_window.update_image();
				init_i2c = true;
				//LL_GPIO_ResetOutputPin(WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin);
				//LL_GPIO_ResetOutputPin(WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin);
				//LL_GPIO_ResetOutputPin(WINDOW_I2C_SCL_RIGHT_GPIO_Port, WINDOW_I2C_SCL_RIGHT_Pin);
				//LL_GPIO_ResetOutputPin(WINDOW_I2C_SDA_RIGHT_GPIO_Port, WINDOW_I2C_SDA_RIGHT_Pin);
			}
		}
	}

	//todo error handling
}
