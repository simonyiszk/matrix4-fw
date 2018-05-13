/*
 * internal_anim.c
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */

#include "../Inc/internal_anim.hpp"
#include "window.hpp"

uint8_t time_to_next_frame = 0;

void internal_animation::step_anim(void){
	static uint32_t i = 0;
	static char szin = 0;

	if(!time_to_next_frame)
		return;

	time_to_next_frame = 0;

	for(size_t k=0;k<windows::num_of_pixels;k++){
		uint8_t j = i << 4;
		windows::right_window.pixels[k].set(szin == 0 ? j : 0, szin == 1 ? j : 0, szin == 2 ? j : 0);
		windows::left_window.pixels[k].set(szin == 0 ? j : 0, szin == 1 ? j : 0, szin == 2 ? j : 0);
	}
	i++;
	if (i == 0x8)
		i = 0;
	if (i == 0)
		szin++;
	if (szin == 3)
		szin = 0;
}
