/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_ircomms.c
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   IR driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_ircomms_init(void) – intialise IR transmmiter
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_ircomms.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Initialise IR transmit
  * @param  None
  * @retval None
  */
extern void s4408736_hal_ircomms_init(){

	unsigned short PrescalerValue;
	// Timer 3 clock enable
	__TIM3_CLK_ENABLE();

	// Compute the prescaler value
	// Set the clock prescaler to 50kHz
	// SystemCoreClock is the system clock frequency
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

	/* TIM Base configuration */
	TIM_InitP.Instance = TIM3;				//Enable Timer 3
	TIM_InitP.Init.Period = 5*(50000/1000);		//Set period count to be 5ms
	TIM_InitP.Init.Prescaler = PrescalerValue;	//Set prescaler value
	TIM_InitP.Init.ClockDivision = 0;			//Set clock division
	TIM_InitP.Init.RepetitionCounter = 0;	// Set reload Value
	TIM_InitP.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up

	/* Initialise Timer */
	HAL_TIM_Base_Init(&TIM_InitP);

	/* Set priority of Timer 3 update Interrupt  */
	/* 	DO NOT SET INTERRUPT PRIORITY HIGHER THAN 3 */
	HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);

	// Enable the Timer 3 interrupt
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	// Start Timer 3 base unit in interrupt mode
	HAL_TIM_Base_Start_IT(&TIM_InitP);

}

