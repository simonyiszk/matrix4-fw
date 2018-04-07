#include "window.h"
#include "main2.h"
#include "main.h"
#include "internal_anim.h"
#include "network.h"

void main2(void){
	init_network();


	while (1){
		step_anim();

		step_window_state(right_window);

		step_network();
	}

	//todo error handling, use DHCP led, use lease ticker
}
