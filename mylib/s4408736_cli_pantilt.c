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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "board.h"
#include "FreeRTOS_CLI.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "stdarg.h"
#include "s4408736_os_cli.h"
#include "s4408736_cli_pantilt.h"
#include "s4408736_os_pantilt.h"

/* Private define ------------------------------------------------------------*/
static BaseType_t prvPanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvTiltCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/* private variables */
CLI_Command_Definition_t xPan = {	/* Structure that defines the "pan" command line command. */
	"pan",
	"pan <Angle Degrees|direction(left or right)>\r\n",
	prvPanCommand,
	1
};

CLI_Command_Definition_t xTilt = {	/* Structure that defines the "pan" command line command. */
	"tilt",
	"tilt <Angle Degrees|direction(left or right)>\r\n",
	prvTiltCommand,
	1
};
/* Private variables ---------------------------------------------------------*/

extern void s4408736_cli_pantilt_init(void) {
	FreeRTOS_CLIRegisterCommand(&xPan);
	FreeRTOS_CLIRegisterCommand(&xTilt);
	//panAngleValue = xQueueCreate(10, sizeof(receivePanAngle));
	//tiltAngleValue = xQueueCreate(10, sizeof(receiveTiltAngle));
}

/**
  * @brief  Pan Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvPanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;
	//int i = 0;
	//int digits = 0;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	if (!strcmp("left", cCmd_string)) {
		xSemaphoreGive(panLeft);
	} else if (!strcmp("right", cCmd_string)) {
		xSemaphoreGive(panRight);
	}
	/*
	else {
		while(i < lParam_len){
			if (isdigit(cCmd_string[i])) {
				digits++;
			}
			i++;
		}
		if (i == digits) {
			sendPanAngle = atoi(cCmd_string);
			if (!(sendPanAngle < -70 || sendPanAngle > 70)) {
				//xQueueSendToFront(panAngleValue, ( void * ) &sendPanAngle, ( portTickType ) 10 );
			}
		}
	}
	*/
	vTaskDelay(1000);
	return pdFALSE;
}

/**
  * @brief  Tilt Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvTiltCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;
	//int i = 0;
	//int digits = 0;
	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	if (!strcmp("down", cCmd_string)) {
		xSemaphoreGive(tiltLeft);
	} else if (!strcmp("up", cCmd_string)) {
		xSemaphoreGive(tiltRight);
	}
	/*
	else {

		while(i < lParam_len){
			if (isdigit(cCmd_string[i])) {
				digits++;
			}
			i++;
		}
		if (i == digits) {
			sendTiltAngle = atoi(cCmd_string);
			if (!(sendTiltAngle < -70 || sendTiltAngle > 70)) {
				//xQueueSendToFront(tiltAngleValue, ( void * ) &sendTiltAngle, ( portTickType ) 10 );
			}
		}
	}
	*/
	vTaskDelay(1000);
	return pdFALSE;
}

