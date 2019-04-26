/*
 * window.hpp
 *
 *  Created on: May 4, 2018
 *      Author: kisada
 */

/******************************************************************************
 *
 *     Window management subsystem for SCH MATRIX4
 *
 *     Kiss Ádám 2018
 *
 *****************************************************************************/

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
#include <stdint.h>
#include "i2c.hpp"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_usart.h"

namespace windows{
	const size_t num_of_pixels = 4;

	struct pixel_data{
		uint8_t red;
		uint8_t green;
		uint8_t blue;

		/*!
		 * \brief Type created for storing the state of the internal buffer
		 * The buffer stores the next differential frame to be sent
		 */
		enum tframe_status{
			buffer_free,
			buffer_full
		};

		enum tframe_status stat;

		pixel_data(); //TODO move to cpp file
		pixel_data(const pixel_data&) = delete;
		pixel_data& operator=(const pixel_data&) = delete;
		//~pixel_data() = delete;

		/*
		 * Sets a pixel for the given color.
		 */
		void set(unsigned char red, unsigned char green, unsigned char blue);

		/*
		 * Sets the stat to free
		 */
		void flush();

		bool isFull();
	};

	class window{
	public:
		/*!
		 * \brief Type created for storing states for each window.
		 *
		 * see documentation for further information
		 */
		enum twindow_status : uint8_t{
			discharge_caps,
			vcc_3v3_off,  //waiting for plug
			vcc_3v3_on,   //waiting for comm
			vcc_12v_off,  //panel turned off remotely
			vcc_12v_on,    //comm ok
			all_init
		};
	private:
		twindow_status status;

		GPIO_TypeDef* gpio_port_3v3;
		GPIO_TypeDef* gpio_port_power; //TODO add const keyword
		uint16_t gpio_pin_3v3, gpio_pin_power; //TODO add const keyword

		DMA_TypeDef* DMAx;
		uint32_t DMA_Channel;
		USART_TypeDef* uart_handler;

		volatile uint8_t DMA_buffer[13];

		bool transmitted_before;

		I2C i2c;
		uint8_t red_addr = 0;
		uint8_t green_addr = 0;
		uint8_t blue_addr = 0;

	public:

		window() = delete;
		window(	GPIO_TypeDef* gpio_port_3v3,
				uint16_t gpio_pin_3v3,
				GPIO_TypeDef* gpio_port_power,
				uint16_t gpio_pin_power,
				USART_TypeDef *USARTx,
				DMA_TypeDef* DMAx,
				uint32_t DMA_Channel);

		window(const window&) = delete;
		window& operator=(const window&) = delete;

		/*!
		 * \brief the the jobs defined by the state of the window
		 *
		 * @param w The window which status is to be changed
		 */
		void step_state();

		/*!
		 * \brief get the status of a window
		 *
		 * @param w which window's status to be returned
		 * @return the status of the window asked in the argument line
		 */
		twindow_status get_state();

		/*!
		 * \brief changes the state of the window passed on the argument line and do the necessary modifications on the board
		 *
		 * @param w The window which status is to be changed
		 */

		void set_state(twindow_status);

		/*!
		 * \brief stores the status of each window
		 */
		std::array<pixel_data, num_of_pixels> pixels;

		bool check_uart_welcome_message();

		void update_image();

		/*!
		 * \brief Blanks every panel connected to the window.
		 */
		void blank();

		//I2C
		//--------------------------------------------------------------------
		bool i2c_panel = false;
		void init_I2C(uint8_t i2c_addr_base, GPIO_TypeDef* SCL_Port, uint32_t SCL_PinMask,
				GPIO_TypeDef* SDA_Port, uint32_t SDA_PinMask,
				TIM_TypeDef* timer, uint32_t F_CLK, uint32_t speed = 100000);
		void init_leds();
		void update_image_i2c();
		void set_group_dim_red(uint8_t dim);
		void set_group_dim_green(uint8_t dim);
		void set_group_dim_blue(uint8_t dim);
	};

	extern window left_window;
	extern window right_window;

	namespace Reg
	{
		enum : uint8_t {
			MODE1			= 0x00,
			MODE2			= 0x01,
			PWM0			= 0x02,
			PWM1			= 0x03,
			PWM2			= 0x04,
			PWM3			= 0x05,
			PWM4			= 0x06,
			PWM5			= 0x07,
			PWM6			= 0x08,
			PWM7			= 0x09,
			PWM8			= 0x0A,
			PWM9			= 0x0B,
			PWM10			= 0x0C,
			PWM11			= 0x0D,
			PWM12			= 0x0E,
			PWM13			= 0x0F,
			PWM14			= 0x10,
			PWM15			= 0x11,
			GRPPWM			= 0x12,
			GRPFREQ			= 0x13,
			LEDOUT0			= 0x14,
			LEDOUT1			= 0x15,
			LEDOUT2			= 0x16,
			LEDOUT3			= 0x17,
			SUBADR1			= 0x18,
			SUBADR2			= 0x19,
			SUBADR3			= 0x1A,
			ALLCALLADR		= 0x1B,
			IREF			= 0x1C,
			EFLAG1			= 0x1D,
			EFLAG2			= 0x1E
		};
	}

	namespace AutoInc {
		enum : uint8_t {
			NONE		= 0b00000000,
			ALL			= 0b10000000,
			PWM			= 0b10100000,
			CTRL		= 0b11000000,
			PWM_CTRL	= 0b11100000
		};
	}

	namespace InitReg {
		enum : uint8_t {
			MODE1		= 0x00,	//OSC: normal mode
			MODE2		= 0x00,
			PWM			= 0x00,	//brightness 0
			GRPPWM		= 0xFF,	//group dim max
			GRPFREQ		= 0x00,	//don't care
			LEDOUT		= 0xFF 	//brightness + group dimming
		};
	}
};




#endif /* WINDOW_HPP_ */
