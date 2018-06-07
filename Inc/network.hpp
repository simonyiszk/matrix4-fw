/*
 * network.h
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

namespace net{
	extern "C" I2C_HandleTypeDef hi2c2;

	class network{
	private:
		network(const network&) = delete;
		network& operator=(const network&) = delete;
		//~network() = delete;

		void do_remote_command();
		void fetch_frame();

		uint8_t status_string[512];

		size_t create_status_string();

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
			delete_anim_network_buffer = 0x06,
			ping = 0x40
		};

		network();
		void step_network();
		void init();
	};

	extern network inetwork;
}

#endif /* NETWORK_HPP_ */
