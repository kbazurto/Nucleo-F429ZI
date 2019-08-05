/**
  ******************************************************************************
  * @file    ex5/main.c
  * @author  Kevin Bazurto
  * @date    02/05/2018
  * @brief   FreeRTOS LED Flashing program using semaphores.Creates a semaphore to
  *			 signal a task to toggle the onboard Blue LED.
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t LEDSemaphore;	/* Semaphore for LED flasher */

/* Private function prototypes -----------------------------------------------*/
static void Hardware_init();
void ApplicationIdleHook( void ); /* The idle hook is just used to stream data to the USB port */
void Give_Task( void );
void Take_Task( void );

/* Task Priorities ------------------------------------------------------------*/
#define mainGIVETASK_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define mainTAKETASK_PRIORITY					( tskIDLE_PRIORITY + 2 )

/* Task Stack Allocations -----------------------------------------------------*/
#define mainGIVETASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define mainTAKETASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )


/**
  * @brief  Starts all the other tasks, then starts the scheduler.
  * @param  None
  * @retval None
  */
int main( void ) {

	BRD_init();
	Hardware_init();

	/* Start task one, two and three. */
    xTaskCreate( (void *) &Give_Task, (const signed char *) "GIVE", mainGIVETASK_STACK_SIZE, NULL, mainGIVETASK_PRIORITY, NULL );
	xTaskCreate( (void *) &Take_Task, (const signed char *) "TAKE", mainTAKETASK_STACK_SIZE, NULL, mainTAKETASK_PRIORITY, NULL );

	/* Create Semaphores */
	LEDSemaphore = xSemaphoreCreateBinary();

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
  * @brief  Give Task.Gives LED Semaphore every second.
  * @param  None
  * @retval None
  */
void Give_Task( void ) {

	for (;;) {


		if (LEDSemaphore != NULL) {	/* Check if semaphore exists */

			/* Give LED Semaphore */
			xSemaphoreGive(LEDSemaphore);

			portENTER_CRITICAL();
			debug_printf("Giving LED Semaphore\n\r");
			portEXIT_CRITICAL();
		}

		/* Wait for 3000ms */
		vTaskDelay(3000);

	}
}


/**
  * @brief  Take Task. Used to take semaphore events. Toggles LED.
  * @param  None
  * @retval None
  */
void Take_Task( void ) {

	uint8_t mode = 1;
	int presscount = 0;

	BRD_LEDBlueOff();

	for (;;) {

		if (LEDSemaphore != NULL) {	/* Check if semaphore exists */

			/* See if we can obtain the LED semaphore. If the semaphore is not available
          	 wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( LEDSemaphore, 10 ) == pdTRUE ) {

				portENTER_CRITICAL();
				debug_printf("Taking LED Semaphore\n\r");
				portEXIT_CRITICAL();

            	/* We were able to obtain the semaphore and can now access the shared resource. */

            	/* Toggle LED, if in correct mode */
				if (mode == 1) {
					BRD_LEDBlueToggle();
				}
        	}
		}

		vTaskDelay(1);
	}
}

/**
  * @brief  Hardware Initialisation.
  * @param  None
  * @retval None
  */
static void Hardware_init( void ) {

	GPIO_InitTypeDef GPIO_InitStructure;

	portDISABLE_INTERRUPTS();	//Disable interrupts

	BRD_LEDInit();				//Initialise Blue LED
	BRD_LEDBlueOff();				//Turn off Blue LED

	portENABLE_INTERRUPTS();	//Enable interrupts
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
  * @brief  Idle Application Task (Disabled)
  * @param  None
  * @retval None
  */
void vApplicationIdleHook( void ) {
	static portTickType xLastTx = 0;

	BRD_LEDBlueOff();

	for (;;) {
		/* The idle hook simply prints the idle tick count */
		if ((xTaskGetTickCount() - xLastTx ) > (1000 / portTICK_RATE_MS)) {
			xLastTx = xTaskGetTickCount();
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

