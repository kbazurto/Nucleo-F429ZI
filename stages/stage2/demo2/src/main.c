/**
  ******************************************************************************
  * @file    demo2/main.c
  * @author  Kevin Bazurto 44087368
  * @date    04/03/2018
  * @brief   PanTilt device driver
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "s4408736_hal_ledbar.h"
#include "s4408736_hal_pantilt.h"
#include "stdlib.h"
#include "stdio.h"
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
TIM_HandleTypeDef TIM_Init1;
/* Private define ------------------------------------------------------------*/
#define PRINTF_REFLECT
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int count_interrupt =0;
int interruptLimit = 2;
int mode = 0;
int metronome = 40;
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);
void TIM2_IRQHandler(void);
void input_handler(char);

/**
  * @brief  Main program - controls interaction between user and pantilt device
  * @param  None
  * @retval None
  */
void main(void)  {
	//Initalise Board, pin, timer and PMW
	BRD_init();
	s4408736_hal_pantilt_init();
	s4408736_hal_ledbar_init();
	Hardware_init();
	//s4408736_hal_pantilt_pan_write(0);
	//used to store user's input
	char command;
	setbuf(stdout, NULL);
	//main loop
	while (1) {
		//HAL_Delay(40);
		//get user input
		command = debug_getc();

		if (command != '\0') {

#ifdef PRINTF_REFLECT
			debug_printf("%c", command);// reflect byte using printf - must delay before calling printf again
#else
			debug_putc(command);        // reflect byte using putc - puts character into buffer
#endif
		}
		input_handler(command);
		debug_printf("angle: %d %c\n\r", s4408736_hal_pantilt_pan_read(), command);
    }
}

/**
  * @brief  Main program - controls user's input and angles
  * @param  input to decide what to do
  * @retval None
  */
void input_handler(char input) {

	int currentAngle = s4408736_hal_pantilt_pan_read();
	int nextAngle = 0;
	//check what mode to use
	if(input == 'n'){
		mode = 0;
	} else if(input == 'm') {
		mode = 1;
	}
	//handle modes
	if (mode == 0) {
		count_interrupt =0;
		s4408736_hal_ledbar_write(0x00);
		//set range between -70 and 70 degrees
		if (input == 'p' && (currentAngle + 10) <= 70) {
			nextAngle = currentAngle + 10;
		} else if(input == 'n' && (currentAngle - 10) >= -70){
			nextAngle = currentAngle - 10;
		} else {
			nextAngle = currentAngle;
		}
		s4408736_hal_pantilt_pan_write(nextAngle);
	} else if (mode == 1){ //handle tempo of metronome
		debug_printf("interrupt: %d limit: %d\n\r", count_interrupt, interruptLimit );
		if (input == '+' && interruptLimit < 20 ) {
			interruptLimit += 1;
		} else if (input == '-' && interruptLimit > 2) {
			interruptLimit -= 1;
		}
	}
}

/**
 * @brief  Initialise Hardware
 * @param  None
 * @retval None
 */
void Hardware_init(void) {
	unsigned short PrescalerValue;
	// Timer 2 clock enable
	__TIM2_CLK_ENABLE();

	// Compute the prescaler value
	// Set the clock prescaler to 50kHz
	// SystemCoreClock is the system clock frequency
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

	/* TIM Base configuration */
	TIM_Init1.Instance = TIM2;				//Enable Timer 2
	TIM_Init1.Init.Period = 2000*(50000/1000);			//Set period count to be 1ms, so timer interrupt occurs every 1ms.
	TIM_Init1.Init.Prescaler = PrescalerValue;	//Set prescaler value
	TIM_Init1.Init.ClockDivision = 0;			//Set clock division
	TIM_Init1.Init.RepetitionCounter = 0;	// Set reload Value
	TIM_Init1.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up.

	/* Initialise Timer */
	HAL_TIM_Base_Init(&TIM_Init1);
	/* Set priority of Timer 2 update Interrupt [0 (HIGH priority) to 15(LOW priority)] */
	/* 	DO NOT SET INTERRUPT PRIORITY HIGHER THAN 3 */
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);		//Set Main priority to 10 and sub-priority to 0.
	// Enable the Timer 2 interrupt
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	// Start Timer 2 base unit in interrupt mode
	HAL_TIM_Base_Start_IT(&TIM_Init1);
}

/**
 * @brief Period elapsed callback in non blocking mode
 * @param htim: Pointer to a TIM_HandleTypeDef that contains the configuration information for the TIM module.
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//handle metronome
	if (htim->Instance == TIM2 && mode == 1) {
		if (count_interrupt >= interruptLimit) {
			if (metronome == 40) {
				s4408736_hal_ledbar_write(0x001);
				metronome = -40;
			} else {
				s4408736_hal_ledbar_write(0x200);
				metronome = 40;
			}
			s4408736_hal_pantilt_pan_write(metronome);
			count_interrupt = 0;
		}
		count_interrupt++;
	}
}

//Override default mapping of this handler to Default_Handler
void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&TIM_Init1);
}
