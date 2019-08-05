/**
 ******************************************************************************
 * @file    mylib/s4408736_os_cli.c
 * @author  Kevin Bazurto – 44087368
 * @date    18/05/2018
 * @brief   Handles CLI
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 *extern void s4408736_cli_init(void) - initialize CLI
 ******************************************************************************
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"
#include "stdarg.h"
#include "s4408736_os_printf.h"
#include "s4408736_os_cli.h"

/* Private define ------------------------------------------------------------*/
void CLI_Task(void);

/* Task Priorities -----------------------------------------------------------*/
#define mainCLI_PRIORITY					( tskIDLE_PRIORITY + 2 )

/* Task Stack Allocations ----------------------------------------------------*/
#define mainCLI_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 3 )

/**
  * @brief  Create cli task
  * @param  None
  * @retval None
  */
extern void s4408736_cli_init(void) {
	xTaskCreate( (void *) &CLI_Task, (const signed char *) "CLI",
			mainCLI_TASK_STACK_SIZE, NULL, mainCLI_PRIORITY, NULL );
}

/**
  * @brief  CLI Receive Task.
  * @param  None
  * @retval None
  */
void CLI_Task(void) {

	char cRxedChar;
	char cInputString[100];
	int InputIndex = 0;
	char *pcOutputString;
	BaseType_t xReturned;

	/* Initialise pointer to CLI output buffer. */
	memset(cInputString, 0, sizeof(cInputString));
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	for (;;) {

		/* Receive character from terminal */
		cRxedChar = debug_getc();

		/* Process if character if not Null */
		if (cRxedChar != '\0') {

			/* Echo character */
			debug_putc(cRxedChar);

			/* Process only if return is received. */
			if (cRxedChar == '\r') {

				//Put new line and transmit buffer
				debug_putc('\n');
				debug_flush();

				/* Put null character in command input string. */
				cInputString[InputIndex] = '\0';

				xReturned = pdTRUE;
				/* Process command input string. */
				while (xReturned != pdFALSE) {

					/* Returns pdFALSE, when all strings have been returned */
					xReturned = FreeRTOS_CLIProcessCommand( cInputString,
							pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

					myprintf("%s\n\r",pcOutputString);

				    vTaskDelay(5);	//Must delay between debug_printfs.
				}

				memset(cInputString, 0, sizeof(cInputString));
				InputIndex = 0;

			} else {

				debug_flush();		//Transmit USB buffer

				if( cRxedChar == '\r' ) {

					/* Ignore the character. */
				} else if( cRxedChar == '\b' ) {

					/* Backspace was pressed.  Erase the last character in the
					 string - if any.*/
					if( InputIndex > 0 ) {
						InputIndex--;
						cInputString[ InputIndex ] = '\0';
					}

				} else {

					/* A character was entered.  Add it to the string
					   entered so far.  When a \n is entered the complete
					   string will be passed to the command interpreter. */
					if( InputIndex < 40 ) {
						cInputString[ InputIndex ] = cRxedChar;
						InputIndex++;
					}
				}
			}
		}

		vTaskDelay(50);
	}
}
