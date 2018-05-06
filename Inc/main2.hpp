/*
 * main2.hpp
 *
 *  Created on: May 4, 2018
 *      Author: kisada
 */

#ifndef MAIN2_HPP_
#define MAIN2_HPP_

#include "main2.h"

enum main_states{
	booting,
	internal_anim,
	external_anim
};

extern enum main_states main_state;

#endif /* MAIN2_HPP_ */
