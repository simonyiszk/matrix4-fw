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

/*!
 * \brief Type created for passing window selection as argument.
 * Since the values are 0 and 1, after a static cast the type can be used as array index.
 * */
enum window_selector{
	left_window  = 0,
	right_window = 1
};

/*!
 * \brief Type created for storing states for each window.
 *
 * see documentation for further information
 */
enum twindow_status{
	discharge_caps,
	vcc_3v3_off,  //waiting for plug
	vcc_3v3_on,   //waiting for comm
	vcc_12v_off,  //panel turned off remotely
	vcc_12v_on    //comm ok
};

/*!
 * \brief Type created for storing the state of the internal buffer
 * The buffer stores the next differential frame to be sent
 */
enum tframe_status{
	buffer_free,
	buffer_full
};

struct pixel_data{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	enum tframe_status stat;
};

//const size_t num_of_pixels = 4;
#define num_of_pixels 4

/*!
 * \brief stores the status of each window
 * Can be indexed using static casted window_selector enum and number of the subpixel
 * Refer documentation
 * TODO do documentation
 *
 * The declaration is in window.c
 */
extern struct pixel_data pixels[2][num_of_pixels];

/*!
 * \brief stores the status of each window
 * Can be indexed using static casted window_selector enum
 *
 * The declaration is in window.c
 */
extern enum tframe_status frame_status[2];

/*!
 * \brief counts the seconds
 * used to countdown the timeout in panel probing
 *
 * The declaration is in window.c
 *Updated in stm32f0xx_it.c (TIM17 INT)
 */
extern uint8_t sec_cntr_window;

/*!
 * \brief Inits the state machine for the window managment system
 */
void init_window_state_machine();

/*!
 * \brief the the jobs defined by the state of the window
 *
 * @param w The window which status is to be changed
 */
void step_window_state(enum window_selector w);

/*!
 * \brief get the status of a window
 *
 * @param w which window's status to be returned
 * @return the status of the window asked in the argument line
 */
enum twindow_status get_window_state(enum window_selector w);

/*!
 * \brief changes the state of the window passed on the argument line and do the necessary modifications on the board
 *
 * @param w The window which status is to be changed
 */
void set_window_state(enum window_selector w , enum twindow_status s); //TODO Think about invalid state changes, and where to filter them


//////////////////,,-------- to be refactored

/*
 * Sets a pixel for the given color.
 */
void set_pixel(enum window_selector w, unsigned char pixel,  unsigned char red, unsigned char green, unsigned char blue);

#endif
