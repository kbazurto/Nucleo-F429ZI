/**
  ******************************************************************************
  * @file    demo3/main.c
  * @author  Kevin Bazurto 44087368
  * @date    11/03/2018
  * @brief   Joystick and IR drivers
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_joystick.h"
#include "s4408736_hal_ir.h"
#include "s4408736_hal_ledbar.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int currentTime = 0;
int lastTime = 0;
int frequency1 = 0;

/* Private function prototypes -----------------------------------------------*/
void hardware_init(void);
void ledbar_handler(int frequency);
void input_handler(char input);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void main(void)  {
	//Initalise Board, pin, timer and PMW
	hardware_init();
	unsigned int x = 0;
	int frequency = 0;
	char command;
	setbuf(stdout, NULL);
	while(1){
		command = debug_getc();
		input_handler(command);
		debug_printf("command: %c\n\r", command);
		x = s4408736_hal_joystick_x_read();
		frequency = (int)((50.0*x)/4095);
		ledbar_handler(frequency);
		debug_printf("X = %X, F = %d\n\r", x, frequency);
		HAL_Delay(1000/(frequency*2));
		s4408736_hal_ir_datamodulation_set();
		HAL_Delay(1000/(frequency*2));
		s4408736_hal_ir_datamodulation_clr();
	}
}

/**
  * @brief  Hadles user's input
  * @param  input: makes decision according to this variable
  * @retval None
*/
void input_handler(char input){
	if (input == 'r') {
		s4408736_hal_ir_init(1);
	} else if (input == 't') {
		s4408736_hal_ir_init(0);
	}
}

/**
  * @brief  Decides what LED to turn on
  * @param  frequency: make decision according to this value
  * @retval None
*/
void ledbar_handler(int frequency){
	int ledValue = 0;
	if (frequency <= 5) {
		ledValue = 1;
	} else if (frequency <= 10) {
		ledValue = 3;
	} else if (frequency <= 15) {
		ledValue = 7;
	} else if (frequency <= 20) {
		ledValue = 15;
	} else if (frequency <= 25) {
		ledValue = 31;
	} else if (frequency <= 30) {
		ledValue = 63;
	} else if (frequency <= 35) {
		ledValue = 127;
	} else if (frequency <= 40) {
		ledValue = 255;
	} else if (frequency <= 45) {
		ledValue = 511;
	} else if (frequency <= 50) {
		ledValue = 1023;
	}
	s4408736_hal_ledbar_write(ledValue);
}

/**
  * @brief  Initialise hardware for stage3
  * @param  None
  * @retval None
*/
void hardware_init(void){
	BRD_init();
	s4408736_hal_joystick_init();
	s4408736_hal_ledbar_init();
	s4408736_hal_ir_init(0);
}

/**
  * @brief  Timer 2 Input Capture Interrupt handler
  * @param  htim: timer handle
  * @retval None
*/
void HAL_TIM2_IRQHandler (TIM_HandleTypeDef *htim) {
	// Clear Input Capture Flag
	__HAL_TIM_CLEAR_IT(&TIM2_Init, TIM_IT_TRIGGER);
	currentTime = HAL_GetTick();
	frequency1 = currentTime - lastTime;
	//debug_printf("period: %d\n\r", frequency1);
	lastTime = currentTime;
}

/**
  * @brief  Timer 2 Input Capture Interrupt handler
  * Override default mapping of this handler to Default_Handler
  * @param  None.
  * @retval None
*/
void TIM2_IRQHandler(void) {
	HAL_TIM2_IRQHandler(&TIM2_Init);
}


