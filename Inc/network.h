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

#endif /* NETWORK_H_ */
