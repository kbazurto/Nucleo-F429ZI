/**
 ******************************************************************************
 * @file    mylib/s4408736_os_pantilt.c
 * @author  Kevin Bazurto – 44087368
 * @date    18/05/2018
 * @brief   Handles pan and tilt movement of servo motors
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "stdarg.h"
#include "s4408736_os_printf.h"
#include "s4408736_os_pantilt.h"
#include "s4408736_hal_pantilt.h"
/* Private define ------------------------------------------------------------*/
void Take_angle_Task( void );
/* Private variables ---------------------------------------------------------*/
int panAngle = 0;
int tiltAngle = 0;


#define angleTAKETASK_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define angleTAKETASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

extern void s4408736_os_pantilt_init(void) {

	xTaskCreate( (void *) &Take_angle_Task, (const signed char *) "TAKE", angleTAKETASK_STACK_SIZE, NULL, angleTAKETASK_PRIORITY, NULL );
	/*initialize semaphores*/
	panRight = xSemaphoreCreateBinary();
	panLeft = xSemaphoreCreateBinary();
	tiltRight = xSemaphoreCreateBinary();
	tiltLeft = xSemaphoreCreateBinary();
}

/**
  * @brief  Take Task. Used to take semaphore events. Toggles LED.
  * @param  None
  * @retval None
  */
void Take_angle_Task( void ) {

	for (;;) {

		if (panRight != NULL) {	/* Check if semaphore exists */

			/* See if we can obtain the LED semaphore. If the semaphore is not available
          	 wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( panRight, 10 ) == pdTRUE ) {
				panAngle = s4408736_hal_pantilt_pan_read();
				s4408736_hal_pantilt_pan_write(panAngle + 5);
				myprintf("pan + 5\r\n");
			}
		}
		if (panLeft != NULL) {	/* Check if semaphore exists */

			/* See if we can obtain the LED semaphore. If the semaphore is not available
          	 wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( panLeft, 10 ) == pdTRUE ) {
				panAngle = s4408736_hal_pantilt_pan_read();
				s4408736_hal_pantilt_pan_write(panAngle - 5);
				myprintf("pan - 5\r\n");
			}
		}
		if (tiltRight != NULL) {	/* Check if semaphore exists */

			/* See if we can obtain the LED semaphore. If the semaphore is not available
          	 wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( tiltRight, 10 ) == pdTRUE ) {
				tiltAngle = s4408736_hal_pantilt_tilt_read();
				s4408736_hal_pantilt_tilt_write(tiltAngle + 5);
				myprintf("tilt + 5\r\n");
			}
		}
		if (tiltLeft != NULL) {	/* Check if semaphore exists */

			/* See if we can obtain the LED semaphore. If the semaphore is not available
          	 wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( tiltLeft, 10 ) == pdTRUE ) {
				tiltAngle = s4408736_hal_pantilt_tilt_read();
				s4408736_hal_pantilt_tilt_write(tiltAngle - 5);
				myprintf("tilt - 5\r\n");
			}
		}
	}
}
