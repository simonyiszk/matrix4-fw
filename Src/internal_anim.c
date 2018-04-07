/*
 * internal_anim.c
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */

#include "internal_anim.h"
#include "window.h"

uint8_t time_to_next_frame = 0;

void step_anim(void){
	static uint32_t i = 0;
	static char szin = 0;

	if(!time_to_next_frame)
		return;

	time_to_next_frame = 0;

	for(size_t j=0;j<2;j++)
		for(size_t k=0;k<num_of_pixels;k++){
			pixels[j][k].red=(szin == 0 ? i : 0);
			pixels[j][k].green=(szin == 1 ? i : 0);
			pixels[j][k].blue=(szin == 2 ? i : 0);
			pixels[j][k].stat = buffer_full;
		}
	i++;
	if (i == 0x8)
		i = 0;
	if (i == 0)
		szin++;
	if (szin == 3)
		szin = 0;
}
