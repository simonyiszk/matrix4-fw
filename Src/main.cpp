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

uint32_t in_mode = 0;

volatile uint8_t alma[2048];

int main(void){
	status::turn_internal_anim_on();
//	windows::left_window.init_I2C(0x60, WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin, WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin, TIM16, 20000000, 100000);
//	windows::right_window.init_I2C(0x60, WINDOW_I2C_SCL_RIGHT_GPIO_Port, WINDOW_I2C_SCL_RIGHT_Pin, WINDOW_I2C_SDA_RIGHT_GPIO_Port, WINDOW_I2C_SDA_RIGHT_Pin, TIM16, 20000000, 100000);
	windows::left_window.preconfig_I2C(0x60, WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin, WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin, TIM16, 20000000, 1000000);
	windows::right_window.preconfig_I2C(0x60, WINDOW_I2C_SCL_RIGHT_GPIO_Port, WINDOW_I2C_SCL_RIGHT_Pin, WINDOW_I2C_SDA_RIGHT_GPIO_Port, WINDOW_I2C_SDA_RIGHT_Pin, TIM16, 20000000, 1000000);

	windows::right_window.config_I2C();
	while (1){
		if(status::if_internal_animation_is_on)
		{
			switch (in_mode) {
			case 0:
				internal_animation::step_anim();
				break;
			case 1:
				internal_animation::step_anim2();
				break;
			default:
				//in_mode = 0;
				break;
			}
		}

		//windows::left_window.i2c.set_SCL();
		//LL_GPIO_SetOutputPin(WINDOW_I2C_SCL_RIGHT_GPIO_Port, WINDOW_I2C_SCL_RIGHT_Pin);
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		inetwork.step_network();
		//LL_GPIO_ResetOutputPin(WINDOW_I2C_SCL_RIGHT_GPIO_Port, WINDOW_I2C_SCL_RIGHT_Pin);
		//windows::left_window.i2c.clear_SCL();
		//HAL_Delay(1);

		if (in_mode != 3 || !status::if_internal_animation_is_on)
		{
			windows::left_window.step_state();
			windows::right_window.step_state();
		}

		if(!LL_GPIO_IsInputPinSet(USER_INPUT_BUTTON_GPIO_Port, USER_INPUT_BUTTON_Pin)){
			while(!LL_GPIO_IsInputPinSet(USER_INPUT_BUTTON_GPIO_Port, USER_INPUT_BUTTON_Pin))
			{
			}
			in_mode++;
			in_mode %= 2;
		}
	}

	//todo error handling
}
