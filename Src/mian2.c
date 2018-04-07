#include "window.h"
#include "main2.h"
#include "main.h"
#include "internal_anim.h"
#include "network.h"

enum main_states main_state = booting;

void main2(void){
	init_network();

	main_state = internal_anim;

	while (1){
		if(main_state == internal_anim)
			step_anim();

		step_window_state(left_window);
		step_window_state(right_window);

		step_network();
	}

	//todo error handling
}
