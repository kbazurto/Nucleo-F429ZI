/********************************************************************************
 * @file    mylib/s4408736_hal_pantilt.h
 * @author  Kevin Bazurto – 44087368
 * @date    04/03/2018
 * @brief   PanTilt device driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_pantilt_init() – intialise PWM function to drive the panTilt device
 ******************************************************************************
 */
#ifndef S4408736_HAL_PANTILT_H_
#define S4408736_HAL_PANTILT_H_

/* Private define ------------------------------------------------------------*/
#define PAN	0
#define TILT 1

//define functions to access internal functions
#define s4408736_hal_pantilt_pan_write(angle)	pantilt_angle_write(PAN, angle)
#define s4408736_hal_pantilt_tilt_write(angle)	pantilt_angle_write(TILT, angle)
#define s4408736_hal_pantilt_pan_read()	pantilt_angle_read(PAN)
#define s4408736_hal_pantilt_tilt_read()	pantilt_angle_read(TILT)

extern void s4408736_hal_pantilt_init(void);
void pantilt_angle_write(int type, int angle);
int pantilt_angle_read(int type);

#endif /* S4408736_HAL_PANTILT_H_ */
