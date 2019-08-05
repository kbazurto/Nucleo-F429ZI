/********************************************************************************
 * @file    mylib/s4408736_hal_ir.h
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   IR driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_ir_init() – intialise IR recevier and transmmiter
 ******************************************************************************
 */

#ifndef S4408736_HAL_IR_H_
#define S4408736_HAL_IR_H_

/* Private define ------------------------------------------------------------*/
#define ON 	1
#define OFF 0
#define s4408736_hal_ir_carrieron()		irhal_carrier(ON)
#define s4408736_hal_ir_carrieroff()	irhal_carrier(OFF)
#define s4408736_hal_ir_datamodulation_set()	HAL_GPIO_WritePin(BRD_D38_GPIO_PORT, BRD_D38_PIN, ON & 0x01)
#define s4408736_hal_ir_datamodulation_clr()	HAL_GPIO_WritePin(BRD_D38_GPIO_PORT, BRD_D38_PIN, OFF & 0x01)

//define functions to access internal functions
extern void s4408736_hal_ir_init();
void irhal_carrier(int state);

#endif /* S4408736_HAL_IR_H_ */
