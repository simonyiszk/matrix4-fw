#include "window.hpp"
#include "main2.hpp"
#include "internal_anim.hpp"
#include "network.hpp"

extern "C"{
#include "main.h"
};

enum main_states main_state = booting;

void main2(void){
	//init_network(); reminder: static instances's constructors are ran before the driver initialization functions

	net::inetwork.init();

	main_state = internal_anim;

	while (1){
		if(main_state == internal_anim)
			internal_animation::step_anim();

		net::inetwork.step_network();

		windows::left_window.step_state();
		windows::right_window.step_state();

		windows::left_window.update_image();
		windows::right_window.update_image();
	}

	//todo error handling
}
