/**
 ******************************************************************************
 * @file    mylib/s4408736_os_printf.c
 * @author  Kevin Bazurto – 44087368
 * @date    18/05/2018
 * @brief   safe printf
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * extern void s4408736_os_printf_init(void) - initialize tasks
 * extern void s44087368_printf (const char *fmt, ...) - prints
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include "s4408736_os_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Private define ------------------------------------------------------------*/
void s4408736_TaskPrintf( void );

/* Task Priorities -----------------------------------------------------------*/
#define PRINTFTASK_PRIORITY				( tskIDLE_PRIORITY + 2 )

/* Task Stack Allocations ----------------------------------------------------*/
#define PRINTFTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 5 )

/* Private variables ---------------------------------------------------------*/
char PayloadSend[1000];
char PayloadReceive[1000];

/**
  * @brief  Create print task and queue
  * @param  None
  * @retval None
  */
extern void s4408736_os_printf_init(void) {

	s4408736_QueuePrintf = xQueueCreate(10, sizeof(PayloadReceive));
	xTaskCreate( (void *) &s4408736_TaskPrintf, (const signed char *) "PRINT",
			PRINTFTASK_STACK_SIZE, NULL, PRINTFTASK_PRIORITY, NULL );
}

/**
  * @brief  Reads value from queue and prints it
  * @param  None
  * @retval None
  */
void s4408736_TaskPrintf( void ) {

	for (;;) {

		if (s4408736_QueuePrintf != NULL) {	/* Check if queue exists */
			/* Check for item received - block atmost for 10 ticks */
			if (xQueueReceive( s4408736_QueuePrintf, &PayloadReceive, 10 )) {
				/* display received item */
				debug_printf("%s", PayloadReceive);
        	}
		}

		vTaskDelay(10);
	}
}

/**
  * @brief  Parameters to be printed
  * @param  None
  * @retval None
  */
extern void s44087368_printf (const char *fmt, ...) {

	va_list args;
	va_start (args, fmt);
	vsprintf (PayloadSend,fmt, args);
	va_end (args);

	//send value to queue
	if (s4408736_QueuePrintf != NULL) {
		xQueueSendToFront(s4408736_QueuePrintf, ( void * ) &PayloadSend,
				( portTickType ) 10 );
	}

}
