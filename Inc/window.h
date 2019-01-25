#ifndef __WINDOW_BLOCKING_H___INCLUDED_____SEM_SCH_BME_HU___
#define __WINDOW_BLOCKING_H___INCLUDED_____SEM_SCH_BME_HU___

/******************************************************************************
 *
 *     Window management subsystem for SCH MATRIX4
 *
 *     Kiss Ádám 2018
 *
 *****************************************************************************/

#include "inttypes.h"
#include <stddef.h>
#include "stm32f0xx_hal.h"
#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif

// 	 extern UART_HandleTypeDef huart1;
 //	 extern UART_HandleTypeDef huart2;


	/*!
	 * \brief counts the seconds
	 * used to countdown the timeout in panel probing
	 *
	 * The declaration is in window.c
	 *Updated in stm32f0xx_it.c (TIM17 INT)
	 */
	extern uint8_t sec_cntr_window;
#ifdef __cplusplus
 }
#endif

#endif
