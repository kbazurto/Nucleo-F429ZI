/********************************************************************************
 * @file    mylib/s4408736_hal_joystick.h
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   Joystick driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * extern void s4408736_hal_joystick_init() – intialise JOYSTICK AND ADC
 ******************************************************************************
 */

#ifndef S4408736_HAL_JOYSTICK_H_
#define S4408736_HAL_JOYSTICK_H_

/* Private define ------------------------------------------------------------*/
ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef AdcChanConfig;
ADC_ChannelConfTypeDef AdcChanConfig1;

//define functions to access internal functions
#define s4408736_hal_joystick_x_read()	joystick_read(AdcChanConfig) //A0 = X
#define s4408736_hal_joystick_y_read()	joystick_read(AdcChanConfig1) //A1 = Y

extern void s4408736_hal_joystick_init(void);
unsigned int joystick_read(ADC_ChannelConfTypeDef channel);

#endif /* S4408736_HAL_JOYSTICK_H_ */
