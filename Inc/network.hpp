/*
 * network.h
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

namespace net{
	class network{
	private:
		network(const network&) = delete;
		network& operator=(const network&) = delete;

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
			get_mac = 0x07,
			use_internal_anim = 0x10,
			use_external_anim = 0x20,
			blank = 0x30,
			delete_anim_network_buffer = 0x06,
			ping = 0x40,
            enable_update=0x50,
            get_new_fw_chksum=0x51,
            refurbish=0x60,
            swap_windows=0x70
		};

		network();
		void step_network();
	};

	extern network inetwork;
}

#endif /* NETWORK_HPP_ */
