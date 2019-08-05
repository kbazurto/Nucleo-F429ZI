/**
  ******************************************************************************
  * @file    demo5/main.c
  * @author  Kevin Bazurto 44087368
  * @date    02/05/2018
  * @brief   System monitor
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "s4408736_hal_sysmon.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "board.h"
#include "debug_printf.h"


/* Task Priorities ------------------------------------------------------------*/
#define mainTask1LED_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainTask2LED_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainTask3LED_PRIORITY				( tskIDLE_PRIORITY + 1 )

/* Task Stack Allocations -----------------------------------------------------*/
#define main_TASK1_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define main_TASK2_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define main_TASK3_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

void Hardware_init( void );
void ApplicationIdleHook( void ); /* The idle hook is used to blink the Blue 'Alive LED' every second */
void Task1_Task( void );
void Task2_Task( void );
void Task3_Task( void );



int main(void)
{

	BRD_init();
	Hardware_init();

	/* Start the task to flash the LED. */
	xTaskCreate( (void *) &Task1_Task, (const signed char *) "TASK1", main_TASK1_STACK_SIZE, NULL, mainTask1LED_PRIORITY, NULL );
	xTaskCreate( (void *) &Task2_Task, (const signed char *) "TASK2", main_TASK2_STACK_SIZE, NULL, mainTask2LED_PRIORITY, NULL );
	xTaskCreate( (void *) &Task3_Task, (const signed char *) "TASK3", main_TASK3_STACK_SIZE, NULL, mainTask3LED_PRIORITY, NULL );
	/* Start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */

	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler. */
	return 0;
}

/**
  * @brief  Task1.
  * @param  None
  * @retval None
  */
void Task3_Task( void ) {
	s4408736_hal_sysmon_chan2_clr(); //Clear LA Channel 2

	for (;;) {
		s4408736_hal_sysmon_chan2_set(); //Set LA Channel 2
		BRD_LEDBlueToggle(); //Random instruction
		vTaskDelay(1); //Extra Delay for 3ms
		s4408736_hal_sysmon_chan2_clr(); //Clear LA Channel 2

	}
}

/**
  * @brief  Task2.
  * @param  None
  * @retval None
  */
void Task2_Task( void ) {
	s4408736_hal_sysmon_chan1_clr(); //Clear LA Channel 1

	for (;;) {
		s4408736_hal_sysmon_chan1_set(); //Set LA Channel 1
		BRD_LEDGreenToggle(); //Random instruction
		vTaskDelay(3); //Extra Delay for 3ms
		s4408736_hal_sysmon_chan1_clr(); //Clear LA Channel 1

		vTaskDelay(1); //Mandatory delay ONLY for
	}
}

/**
  * @brief  Task1.
  * @param  None
  * @retval None
  */
void Task1_Task( void ) {
	s4408736_hal_sysmon_chan0_clr(); //Clear LA Channel 0

	for (;;) {
		s4408736_hal_sysmon_chan0_set(); //Set LA Channel 0
		BRD_LEDRedToggle(); //Random instruction
		vTaskDelay(3); //Extra Delay for 3ms
		s4408736_hal_sysmon_chan0_clr(); //Clear LA Channel 0

		vTaskDelay(1); //Mandatory delay ONLY for
	}
}






/**
  * @brief  Hardware Initialisation.
  * @param  None
  * @retval None
  */
void Hardware_init( void ) {

	portDISABLE_INTERRUPTS();	//Disable interrupts
	BRD_LEDInit();		//Initialise LEDS
	s4408736_hal_sysmon_init();

	/* Turn off LEDs and pins*/
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();


	portENABLE_INTERRUPTS();	//Enable interrupts

}

/**
  * @brief  Application Tick Task.
  * @param  None
  * @retval None
  */
void vApplicationTickHook( void ) {

	BRD_LEDGreenOff();
}

/**
  * @brief  Idle Application Task
  * @param  None
  * @retval None
  */
void vApplicationIdleHook( void ) {
	static portTickType xLastTx = 0;

	BRD_LEDGreenOff();

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

	//BRD_LEDBlueOff();
	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
