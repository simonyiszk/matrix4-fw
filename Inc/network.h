/*
 * network.h
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */

#ifndef NETWORK_H_
#define NETWORK_H_


#include "stm32f0xx_hal.h"

extern I2C_HandleTypeDef hi2c2;

extern SPI_HandleTypeDef hspi1;

void init_network();

void step_network();

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

#endif /* NETWORK_H_ */
