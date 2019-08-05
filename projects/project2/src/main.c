/**
  ******************************************************************************
  * @file    pproject2/main.c
  * @author  Kevin Bazurto
  * @date    20/05/2018
  * @brief   project2
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

/*Mylib include*/
#include "s4408736_os_printf.h"
#include "s4408736_os_cli.h"
#include "s4408736_os_APCRadio.h"
#include "s4408736_cli_APCShell.h"
#include "s4408736_hal_radio.h"
#include "s4408736_os_APCDisplay.h"
#include "s4408736_hal_pantilt.h"

/* Private function prototypes -----------------------------------------------*/
void Hardware_init();
void ApplicationIdleHook( void );

/**
  * @brief  Starts all the other tasks, then starts the scheduler.
  * @param  None
  * @retval None
  */
int main( void ) {

	BRD_init();
	Hardware_init();
	vTaskStartScheduler();

  	return 0;
}


/**
  * @brief  Hardware Initialisation.
  * @param  None
  * @retval None
  */
void Hardware_init( void ) {

	portENTER_CRITICAL();

	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	portEXIT_CRITICAL();

	s4408736_hal_radio_init();
	s4408736_hal_pantilt_init();
	s4408736_os_printf_init();
	s4408736_cli_init();
	s4408736_cli_apcshell_init();
	s4408736_os_apcradio_init();
	s4408736_os_apcdisplay_init();
}

/**
  * @brief  Application Tick Task.
  * @param  None
  * @retval None
  */
void vApplicationTickHook( void ) {

	BRD_LEDBlueOff();
}

/**
  * @brief  Idle Application Task
  * @param  None
  * @retval None
  */
void vApplicationIdleHook( void ) {
	static portTickType xLastTx = 0;

	BRD_LEDBlueOff();

	for (;;) {

		/* The idle hook simply prints the idle tick count, every second */
		if ((xTaskGetTickCount() - xLastTx ) > (1000 / portTICK_RATE_MS)) {

			xLastTx = xTaskGetTickCount();

			/* Blink Alive LED */
			BRD_LEDBlueToggle();
		}
	}
}

/**
  * @brief  vApplicationStackOverflowHook
  * @param  Task Handler and Task Name
  * @retval None
  */
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName ) {
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */

	BRD_LEDBlueOff();
	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
