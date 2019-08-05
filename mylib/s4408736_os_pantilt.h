/********************************************************************************
 * @file    mylib/s4408736_os_pantilt.h
 * @author  Kevin Bazurto – 44087368
 * @date    18/05/2018
 * @brief   Handles pan and tilt movement of servo motors
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef S4408736_OS_PANTILT_H_
#define S4408736_OS_PANTILT_H_

/* Private define ------------------------------------------------------------*/

/*variables*/
SemaphoreHandle_t panRight;
SemaphoreHandle_t panLeft;
SemaphoreHandle_t tiltRight;
SemaphoreHandle_t tiltLeft;

//QueueHandle_t panAngleValue;
//QueueHandle_t tiltAngleValue;
int sendPanAngle;
int receivePanAngle;
int sendTiltAngle;
int receiveTiltAngle;

//define functions to access internal functions
extern void s4408736_os_pantilt_init(void);

#endif /* S4408736_OS_PANTILT_H_ */

