/********************************************************************************
 * @file    mylib/s4408736_hal_ircomms.h
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   IR trasnmit driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_ircomms_init() – intialise IR transmmiter
 ******************************************************************************
 */

#ifndef S4408736_HAL_IRCOMMS_H_
#define S4408736_HAL_IRCOMMS_H_

/* Private define ------------------------------------------------------------*/
TIM_HandleTypeDef TIM_InitP;

//define functions to access internal functions
extern void s4408736_hal_ircomms_init();


#endif /* S4408736_HAL_IRCOMMS_H_ */
