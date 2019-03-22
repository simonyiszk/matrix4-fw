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
			vcc_12v_on    //comm ok
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

		const uint8_t REG_MODE1			= (0x00);
		const uint8_t REG_MODE2			= (0x01);
		const uint8_t REG_PWM0			= (0x02);
		const uint8_t REG_PWM1			= (0x03);
		const uint8_t REG_PWM2			= (0x04);
		const uint8_t REG_PWM3			= (0x05);
		const uint8_t REG_PWM4			= (0x06);
		const uint8_t REG_PWM5			= (0x07);
		const uint8_t REG_PWM6			= (0x08);
		const uint8_t REG_PWM7			= (0x09);
		const uint8_t REG_PWM8			= (0x0A);
		const uint8_t REG_PWM9			= (0x0B);
		const uint8_t REG_PWM10			= (0x0C);
		const uint8_t REG_PWM11			= (0x0D);
		const uint8_t REG_PWM12			= (0x0E);
		const uint8_t REG_PWM13			= (0x0F);
		const uint8_t REG_PWM14			= (0x10);
		const uint8_t REG_PWM15			= (0x11);
		const uint8_t REG_GRPPWM		= (0x12);
		const uint8_t REG_GRPFREQ		= (0x13);
		const uint8_t REG_LEDOUT0		= (0x14);
		const uint8_t REG_LEDOUT1		= (0x15);
		const uint8_t REG_LEDOUT2		= (0x16);
		const uint8_t REG_LEDOUT3		= (0x17);
		const uint8_t REG_SUBADR1		= (0x18);
		const uint8_t REG_SUBADR2		= (0x19);
		const uint8_t REG_SUBADR3		= (0x1A);
		const uint8_t REG_ALLCALLADR	= (0x1B);
		const uint8_t REG_IREF			= (0x1C);
		const uint8_t REG_EFLAG1		= (0x1D);
		const uint8_t REG_EFLAG2		= (0x1E);

		const uint8_t AUTO_INC_NONE		= (0b00000000);
		const uint8_t AUTO_INC_ALL		= (0b10000000);
		const uint8_t AUTO_INC_PWM		= (0b10100000);
		const uint8_t AUTO_INC_CTRL		= (0b11000000);
		const uint8_t AUTO_INC_PWM_CTRL	= (0b11100000);

		const uint8_t INIT_MODE1		= 0x00;	//OSC: normal mode
		const uint8_t INIT_MODE2		= 0x00;
		const uint8_t INIT_PWM			= 0x00;	//brightness 0
		const uint8_t INIT_GRPPWM		= 0xFF;	//group dim max
		const uint8_t INIT_GRPFREQ		= 0x00;	//don't care
		const uint8_t INIT_LEDOUT		= 0xFF; 	//brightness + group dimming
	};

	extern window left_window;
	extern window right_window;
};


#endif /* WINDOW_HPP_ */
