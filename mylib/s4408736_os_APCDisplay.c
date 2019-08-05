/**
 ******************************************************************************
 * @file    mylib/s4408736_os_APCDisplay.c
 * @author  Kevin Bazurto – 44087368
 * @date    30/05/2018
 * @brief   Handles pan and tilt movement of servo motors
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_os_apcdisplay_init(void) - initiliaze tasks
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
#include "s4408736_os_APCDisplay.h"
#include "s4408736_hal_pantilt.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private define ------------------------------------------------------------*/
void s4408736_TaskPanTilt( void );

/* Private variables ---------------------------------------------------------*/
uint8_t xyCoordinates[6] = {'0','0','0','0','0','0'};
uint8_t xCoordinate[3] = {'0','0','0'};
uint8_t yCoordinate[3] = {'0','0','0'};

#define PANTILTTASK_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define PANTILTTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

/**
  * @brief  Create task for pan and tilt
  * @param  None
  * @retval None
  */
extern void s4408736_os_apcdisplay_init(void) {

	xTaskCreate( (void *) &s4408736_TaskPanTilt, (const signed char *) "SERVO",
			PANTILTTASK_STACK_SIZE, NULL, PANTILTTASK_PRIORITY, NULL );

	/*initialize semaphores*/
	s4408736_QueuePanTiltMove = xQueueCreate(10, sizeof(xyCoordinates));

}

/**
  * @brief  Get X and Y coordinates
  * @param  xyArray - array with format [X,X,X,Y,Y,Y]
  * @retval None
  */
void separate_position(uint8_t* xyArray) {

	for (int i = 0; i < 6; i++) {

		if (i >= 0 && i < 3) {
			xCoordinate[i] = xyArray[i];
		} else {
			yCoordinate[i - 3] = xyArray[i];
		}
	}
}

/**
  * @brief  Converts position to angle
  * @param  xArray = x Position
  * @retval None
  */
int map_x(uint8_t *xArray) {

	int x = atoi(xArray);
	int angle = ((-44 * x) / 200.0) + 40;
	return angle;
}

/**
  * @brief  Converts position to angle
  * @param  yArray = y Position
  * @retval None
  */
int map_y(uint8_t *yArray) {

	int y = atoi(yArray);
	int angle = ((1* y) / 10.0) - 85;
	return angle;
}

/**
  * @brief  Pan and Tilt task. Use Queues to communicate between tasks
  * @param  None
  * @retval None
  */
void s4408736_TaskPanTilt( void ) {

	for (;;) {

		if (xQueueReceive( s4408736_QueuePanTiltMove, &xyCoordinates, 10 )) {

			separate_position(xyCoordinates);

			//Variables used to decide hw t move the servo
			int lastPanAngle = s4408736_hal_pantilt_pan_read();
			int lastTiltAngle = s4408736_hal_pantilt_tilt_read();
			int panAngle = map_x(xCoordinate);
			int tiltAngle = map_y(yCoordinate);
			int panDistance = panAngle - lastPanAngle;
			int tiltDistance = tiltAngle - lastTiltAngle;

			//new x angle is bigger
			if (panDistance > 0) {

				for (int i = lastPanAngle; i <= panAngle; i++) {
					s4408736_hal_pantilt_pan_write(i);
					vTaskDelay(100);
				}
			// new x angle is smaller
			} else if (panDistance < 0) {

				for (int i = lastPanAngle; i >= panAngle; i--) {
					s4408736_hal_pantilt_pan_write(i);
					vTaskDelay(100);
				}
			}

			//new y angle is bigger
			if (tiltDistance > 0) {

				for (int i = lastTiltAngle; i <= tiltAngle; i++) {
					s4408736_hal_pantilt_tilt_write(i);
					vTaskDelay(100);
				}

			// new y angle is smaller
			} else if (tiltDistance < 0) {

				for (int i = lastTiltAngle; i >= tiltAngle; i--) {
					s4408736_hal_pantilt_tilt_write(i);
					vTaskDelay(100);
				}
			}
		}
		vTaskDelay(950);
	}
}
