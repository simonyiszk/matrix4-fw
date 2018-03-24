#ifndef  __WINDOW_BLOCKING_H___INCLUDED_____SEM_SCH_BME_HU___
#define __WINDOW_BLOCKING_H___INCLUDED_____SEM_SCH_BME_HU___

enum window_selector{
	left_window,
	right_window
};

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
