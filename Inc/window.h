#ifndef  __WINDOW_BLOCKING_H___INCLUDED_____SEM_SCH_BME_HU___
#define __WINDOW_BLOCKING_H___INCLUDED_____SEM_SCH_BME_HU___

#include "inttypes.h"

enum window_selector{
	left_window  = 0,
	right_window = 1
};

enum twindow_status{
	vcc_3v3_off,
	vcc_3v3_on, //waiting for comm, or  communication failed
	comm_ok,
	vcc_12v_off,
	vcc_12v_on
};

enum tframe_status{
	buffer_free,
	buffer_full
};

//localted in window.c
extern enum twindow_status window_status[2];
extern enum tframe_status frame_status[2];
extern uint8_t sec_cntr_window;//incremented in sec interrupt


void init_window_state_machine();

void step_window_state(enum window_selector w);
enum twindow_status get_window_state(enum window_selector w);
void set_window_state(enum window_selector w , enum twindow_status s); //TODO Think about invalid state changes, and where to filter them





/*
 * Checks if the window is set online
 *
 * Returns 1 if PWM board has its TX pin pulled up.
 */
char if_window(enum window_selector w);

/*
 * Turns 12V on the selected channel
 */
void turn_12V_on(enum window_selector w);

/*
 * Turns 12V off the selected channel
 */
void turn_12V_off(enum window_selector w);

/*
 * Sets a pixel for the given color.
 */
void set_pixel(enum window_selector w, unsigned char pixel,  unsigned char red, unsigned char green, unsigned char blue);

#endif
