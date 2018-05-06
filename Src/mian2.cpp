#include "window.h"
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

		windows::left_window.step_state();
		windows::right_window.step_state();

		net::inetwork.step_network();
	}

	//todo error handling
}
