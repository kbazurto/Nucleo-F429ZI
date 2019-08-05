/********************************************************************************
 * @file    mylib/s4408736_os_APCDisplay.h
 * @author  Kevin Bazurto – 44087368
 * @date    30/05/2018
 * @brief   Handles pan and tilt movement of servo motors
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *	extern void s4408736_os_apcdisplay_init(void) - initialize task
 ******************************************************************************
 */

#ifndef S4408736_OS_APCDISPLAY_H_
#define S4408736_OS_APCDISPLAY_H_

QueueHandle_t s4408736_QueuePanTiltMove; //Queue used to move servo motors

//define functions to access internal functions
extern void s4408736_os_apcdisplay_init(void);

#endif /* S4408736_OS_APCDISPLAY_H_ */

