/*
 * window.hpp
 *
 *  Created on: May 4, 2018
 *      Author: kisada
 */

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
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

		void init();

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
		 *  Preventing non-static instancing.
		 */
		//~window() = delete;

		/*!
		 * \brief Blanks every panel connected to the window.
		 */
		void blank();
	};

	extern window left_window;
	extern window right_window;
};


#endif /* WINDOW_HPP_ */
