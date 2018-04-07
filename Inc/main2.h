#ifndef MAIN2_H_____SEM_SCH_BME_HU___
#define MAIN2_H_____SEM_SCH_BME_HU___

#include "stm32f0xx_hal.h"

void main2(void);

enum main_states{
	booting,
	internal_anim,
	external_anim
};

extern enum main_states main_state;

#endif //MAIN2_H_____SEM_SCH_BME_HU___
