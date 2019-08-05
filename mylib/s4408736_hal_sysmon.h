/********************************************************************************
 * @file    mylib/s4408736_hal_sysmon.h
 * @author  Kevin Bazurto – 44087368
 * @date    02/05/2018
 * @brief   handle pins
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_sysmon_init(void) – intialise pins d10 d11 d12
 ******************************************************************************
 */

#ifndef S4408736_HAL_SYSMON_H_
#define S4408736_HAL_SYSMON_H_

/* Private define ------------------------------------------------------------*/
#define ON 	1
#define OFF 0

#define s4408736_hal_sysmon_chan0_set()	HAL_GPIO_WritePin(BRD_D10_GPIO_PORT, BRD_D10_PIN, ON & 0x01)
#define s4408736_hal_sysmon_chan0_clr()	HAL_GPIO_WritePin(BRD_D10_GPIO_PORT, BRD_D10_PIN, OFF & 0x01)
#define s4408736_hal_sysmon_chan1_set()	HAL_GPIO_WritePin(BRD_D11_GPIO_PORT, BRD_D11_PIN, ON & 0x01)
#define s4408736_hal_sysmon_chan1_clr()	HAL_GPIO_WritePin(BRD_D11_GPIO_PORT, BRD_D11_PIN, OFF & 0x01)
#define s4408736_hal_sysmon_chan2_set()	HAL_GPIO_WritePin(BRD_D12_GPIO_PORT, BRD_D12_PIN, ON & 0x01)
#define s4408736_hal_sysmon_chan2_clr()	HAL_GPIO_WritePin(BRD_D12_GPIO_PORT, BRD_D12_PIN, OFF & 0x01)

//define functions to access internal functions
extern void s4408736_hal_sysmon_init();

#endif /* S4408736_HAL_SYSMON_H_ */
