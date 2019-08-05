/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_ledbar.h
 * @author  Kevin Bazurto – 44087368
 * @date    01/03/2018
 * @brief   LED Light Bar peripheral driver
 *	     REFERENCE: LEDLightBar_datasheet.pdf
 *
 *			NOTE: REPLACE sxxxxxxx with your student login.
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_ledbar_init() – intialise LED Light BAR
 * s4408736_hal_ledbar_write() – set LED Light BAR value
 ******************************************************************************
 */

#ifndef S4408736_hal_ledbar_H
#define S4408736_hal_ledbar_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* External function prototypes ----------------------------------------------*/

extern void s4408736_hal_ledbar_init(void);
extern void s4408736_hal_ledbar_write(unsigned short value);
#endif
