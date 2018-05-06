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
	public:
		enum commands{
			turn_12v_off_left,
			turn_12v_off_right,
			reset_left_panel,
			reset_right_panel,
			reboot,
			get_status,
			use_internal_anim = 0x10,
			use_external_anim = 0x20,
			blank = 0x30
		};

		network();
		void step_network();
		void init();
	};

	extern network inetwork;
}

#endif /* NETWORK_HPP_ */
