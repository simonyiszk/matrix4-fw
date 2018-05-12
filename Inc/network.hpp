/*
 * network.h
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include "stm32f0xx_hal.h"


namespace net{
	extern "C" I2C_HandleTypeDef hi2c2;

	extern "C" SPI_HandleTypeDef hspi1;


	class network{
	private:
		network(const network&) = delete;
		network& operator=(const network&) = delete;
		//~network() = delete;

		void do_remote_command();
		void fetch_frame();

	public:
		enum commands{
			turn_12v_off_left = 0x00,
			turn_12v_off_right = 0x01,
			reset_left_panel  = 0x02,
			reset_right_panel = 0x03,
			reboot  = 0x04,
			get_status = 0x05,
			use_internal_anim = 0x10,
			use_external_anim = 0x20,
			blank = 0x30,
			delete_anim_network_buffer = 0x06
		};

		network();
		void step_network();
		void init();

		void link_status_to_joker_led();
	};

	extern network inetwork;
}

#endif /* NETWORK_HPP_ */
