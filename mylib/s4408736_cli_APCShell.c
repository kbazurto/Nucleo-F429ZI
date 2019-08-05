/**
 ******************************************************************************
 * @file    mylib/s4408736_cli_APCShell.c
 * @author  Kevin Bazurto – 44087368
 * @date    20/05/2018
 * @brief   Handles CLI
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 *extern void s4408736_cli_apcshell_init(void) - initialize CLI commands
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
#include "s4408736_os_printf.h"
#include "s4408736_cli_APCShell.h"
#include "s4408736_os_APCRadio.h"


/* Private prototype function define -----------------------------------------*/
static BaseType_t prvGetSysCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvSetChanCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvGetChanCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvSetTxAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvSetRxAddrtCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvGetRxAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvGetTxAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvJoinCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvMoveCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvPenCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvSetHeightCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvOriginCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvLineCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvSquareCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvBlineCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString );

/* Private variables ---------------------------------------------------------*/

/* Structures that defines the commands for the cli */

CLI_Command_Definition_t xGetSys = {
	"getsys",
	"getsys <> Print the current system time (ms).\r\n",
	prvGetSysCommand,
	0
};

CLI_Command_Definition_t xSetChan = {
	"setchan",
	"setchan <chan> Set radio channel to chan.\r\n",
	prvSetChanCommand,
	1
};

CLI_Command_Definition_t xGetChan = {
	"getchan",
	"getchan <> Print the radio channel as read from radio.\r\n",
	prvGetChanCommand,
	0
};

CLI_Command_Definition_t xSetTxAddr = {
	"settxaddr",
	"settxaddr <TXaddress> Set radio TX address to TXaddr in HEX.\r\n",
	prvSetTxAddrCommand,
	1
};

CLI_Command_Definition_t xSetRxAddr = {
	"setrxaddr",
	"setrxaddr <RXaddress> Set radio RX address to RXaddr in HEX.\r\n",
	prvSetRxAddrtCommand,
	1
};

CLI_Command_Definition_t xGetRxAddr = {
	"getrxaddr",
	"getrxaddr <> Print the radio TX address as read from radio in HEX.\r\n",
	prvGetRxAddrCommand,
	0
};

CLI_Command_Definition_t xGetTxAddr = {
	"gettxaddr",
	"gettxaddr <> Print the radio RX address as read from radio in HEX.\r\n",
	prvGetTxAddrCommand,
	0
};

CLI_Command_Definition_t xJoin = {
	"join",
	"join <> Sends a join packet to a plotter, to ensure exclusive use of the "
	"plotter by the sender.\r\n",
	prvJoinCommand,
	0
};

CLI_Command_Definition_t xMove = {
	"move",
	"move <x, y> Move the plotter head to the X, Y coordinates (in mm)Max "
	"value is 200mm\r\n",
	prvMoveCommand,
	2
};

CLI_Command_Definition_t xPen = {
	"pen",
	"pen <up or down> Move the plotter head up or down. Max value is 50mm.\r\n",
	prvPenCommand,
	1
};

CLI_Command_Definition_t xSetHeight = {
	"setheight",
	"setheight <height> Set value for pen down\r\n",
	prvSetHeightCommand,
	1
};

CLI_Command_Definition_t xOrigin = {
	"origin",
	"origin <> Send an XYZ packet to move the plotter’s head to [0,0,0].\r\n",
	prvOriginCommand,
	0
};

CLI_Command_Definition_t xLine = {
	"line",
	"line x1, y1, <type h/v>, length Draw a horizontal or vertical straight "
	"line from (x1,y1) with a specified length in mm\r\n",
	prvLineCommand,
	4
};

CLI_Command_Definition_t xSquare = {
	"square",
	"square x1, y1, Draw a square with starting corner at (x1,y1) with side "
	"length.\r\n",
	prvSquareCommand,
	3
};

CLI_Command_Definition_t xBline = {
	"bline",
	"bline x1, y1, x2, y2, step size Draw a line using the Bresenham line "
	"drawing algorithm. Step size is in mm.\r\n",
	prvBlineCommand,
	5
};

/**
  * @brief  Register commands for CLI
  * @param  None
  * @retval None
  */
extern void s4408736_cli_apcshell_init(void) {

	FreeRTOS_CLIRegisterCommand(&xGetSys);
	FreeRTOS_CLIRegisterCommand(&xSetChan);
	FreeRTOS_CLIRegisterCommand(&xGetChan);
	FreeRTOS_CLIRegisterCommand(&xSetTxAddr);
	FreeRTOS_CLIRegisterCommand(&xSetRxAddr);
	FreeRTOS_CLIRegisterCommand(&xGetRxAddr);
	FreeRTOS_CLIRegisterCommand(&xGetTxAddr);
	FreeRTOS_CLIRegisterCommand(&xJoin);
	FreeRTOS_CLIRegisterCommand(&xMove);
	FreeRTOS_CLIRegisterCommand(&xPen);
	FreeRTOS_CLIRegisterCommand(&xSetHeight);
	FreeRTOS_CLIRegisterCommand(&xOrigin);
	FreeRTOS_CLIRegisterCommand(&xLine);
	FreeRTOS_CLIRegisterCommand(&xSquare);
	FreeRTOS_CLIRegisterCommand(&xBline);
}

/**
  * @brief  Casting from str to int
  * @param  buffer = fill with result,lenght = use to detect if last digit is 0
  *  		cCmd_string = string;
  * @retval None
  */
void set_address(uint8_t *buffer, long lenght, const char *cCmd_string) {

	if (lenght%2 == 0) {

		buffer[3] = ((cCmd_string[1] - 48) | (cCmd_string[0] - 48) << 4);
		buffer[2] = ((cCmd_string[3] - 48) | (cCmd_string[2] - 48) << 4);
		buffer[1] = ((cCmd_string[5] - 48) | (cCmd_string[4] - 48) << 4);
		buffer[0] = ((cCmd_string[7] - 48) | (cCmd_string[6] - 48) << 4);

	} else {

		buffer[3] = ((cCmd_string[0] - 48));
		buffer[2] = ((cCmd_string[2] - 48) | (cCmd_string[1] - 48) << 4);
		buffer[1] = ((cCmd_string[4] - 48) | (cCmd_string[3] - 48) << 4);
		buffer[0] = ((cCmd_string[6] - 48) | (cCmd_string[5] - 48) << 4);

	}
}

/**
  * @brief  Set limit of coordinate to 200 and format the array correctly
  * @param  None
  * @retval None
  */
void check_maximun_position(char *position) {

	if (position[0] > 50) {

		position[0] = '2';
		position[1] = '0';
		position[2] = '0';

	}
	if (position[0] == 50 && (position[1] > 48 || position[2] > 48)) {

		position[1] = '0';
		position[2] = '0';

	}
}

/**
  * @brief  Append coordinates into one buffer
  * @param  xyPosition = xPosition + yPosition
  * @retval None
  */
void fill_xy_position(char *xPosition, char *yPosition, char *xyPosition) {

	for (int i = 0; i < 6; i++) {

		if (i < 3) {
			xyPosition[i] = xPosition[i];
		} else {
			xyPosition[i] = yPosition[i - 3];
		}

	}
}

/**
  * @brief  Change order of coordinate characters
  * @param  result = to decide if coordinates need changes
  * @retval None
  */
void format_packet(int result, char *coordinate) {

	if (result < 10) {
		coordinate[2] = coordinate[0];
		coordinate[1] = '0';
		coordinate[0] = '0';
	} else if (result < 100) {
		coordinate[2] = coordinate[1];
		coordinate[1] = coordinate[0];
		coordinate[0] = '0';
	}
}

/**
  * @brief  Function to generate x and y coordinates for bline
  * @param  x0,y0,x1,y1,stepSize use to generate coordinates
  * @retval None
  */
void generate_max_to_min(int x0,int y0, int x1, int y1, int stepSize) {

	//store X and Y coordinates
	char blineX[3];
	char blineY[3];
	char blineXY[6];

	//Used to calculate coordinates
	int xDistance = x1 - x0;
	int yDistance = y1 - y0;
	int direction = 1;

	if (yDistance < 0) {
		direction = -1;
		yDistance = -yDistance;
	}

	int totalDistance = 2*yDistance - xDistance;
	int y = y0;

	//generate coordinates
	for (int i = x0; i <= x1; i = i + stepSize) {

		//Plot lines
		sprintf(blineX, "%d", i);
		sprintf(blineY, "%d", y);
		format_packet(y, blineY);
		format_packet(i, blineX);
		fill_xy_position(blineX, blineY, blineXY);

		//Send to plotter
		xQueueSendToBack(s4408736_QueueRadioGraphics, ( void * ) &blineXY,
				( portTickType ) 10 );

		if (totalDistance > 0) {

			y = y + (direction * stepSize);
			totalDistance = totalDistance - 2*xDistance;
	    }

		totalDistance = totalDistance + 2*yDistance;
	}
}

/**
  * @brief  Function to generate x and y coordinates for bline
  * @param  x0,y0,x1,y1,stepSize use to generate coordinates
  * @retval None
  */
void generate_min_to_max(int x0, int y0, int x1, int y1, int stepSize) {

	//store X and Y coordinates
	char blineX[3];
	char blineY[3];
	char blineXY[6];

	//Used to calculate coordinates
	int xDistance = x1 - x0;
	int yDistance = y1 - y0;
	int direction = 1;

	if (xDistance < 0) {

		direction = -1;
		xDistance = -xDistance;
	}

	int totalDistance = 2*xDistance - yDistance;
	int x = x0;

	//generate coordinates
	for (int i = y0; i <= y1; i = i + stepSize) {

		//Plot lines
		sprintf(blineX, "%d" ,x);
		sprintf(blineY, "%d",i);
		format_packet(x, blineX);
		format_packet(i, blineY);
		fill_xy_position(blineX, blineY, blineXY);

		//Send to plotter
		xQueueSendToBack(s4408736_QueueRadioGraphics, ( void * ) &blineXY,
				( portTickType ) 10 );

		if (totalDistance > 0) {

			x = x + (direction * stepSize);
			totalDistance = totalDistance - 2*yDistance;
		}

		totalDistance = totalDistance + 2*xDistance;
	}
}

/**
  * @brief  Generate coordinates for bline and send it to a queue
  * @param  x0,y0,x1,y1,step use to generate coordinates
  * @retval None
  */
void generate_bline_coordinates(int x0coordinate, int x1coordinate,
		int y0coordinate, int y1coordinate, int step) {

	//Decide if slope is positive or negative
	if (abs(y1coordinate - y0coordinate) < abs(x1coordinate - x0coordinate)) {

		//Decide what direction to draw
		if (x0coordinate > x1coordinate) {

			generate_max_to_min(x1coordinate, y1coordinate, x0coordinate,
					y0coordinate,step);

		} else {

			generate_max_to_min(x0coordinate, y0coordinate, x1coordinate,
					y1coordinate, step );
		}

	} else {

		//Decide what direction to draw
		if (y0coordinate > y1coordinate) {

			generate_min_to_max(x1coordinate, y1coordinate, x0coordinate,
					y0coordinate, step);

		} else {

			generate_min_to_max(x0coordinate, y0coordinate, x1coordinate,
					y1coordinate, step);

		}
	}
}

/**
  * @brief  getsys Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvGetSysCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;
	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	xSemaphoreGive(s4408736_SemaphoreRadioGetSys);

	return pdFALSE;
}

/**
  * @brief  setchan Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvSetChanCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	char setChannel = atoi(cCmd_string);

	xQueueSendToFront(s4408736_QueueRadioChannel, ( void * ) &setChannel,
			( portTickType ) 10 );

	return pdFALSE;
}

/**
  * @brief  getchan Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvGetChanCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	xSemaphoreGive(s4408736_SemaphoreRadioGetChan);

	return pdFALSE;
}

/**
  * @brief  settxaddr Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvSetTxAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;
	uint8_t txAddr[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	set_address(txAddr,lParam_len, cCmd_string);

	xQueueSendToFront(s4408736_QueueRadioTxAddr, ( void * ) &txAddr,
			( portTickType ) 10 );

	return pdFALSE;
}

/**
  * @brief  setrxaddr Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvSetRxAddrtCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;
	uint8_t rxAddr[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	set_address(rxAddr,lParam_len, cCmd_string);

	xQueueSendToFront(s4408736_QueueRadioRxAddr, ( void * ) &rxAddr,
			( portTickType ) 10 );

	return pdFALSE;
}

/**
  * @brief  getrxaddr Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvGetRxAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	xSemaphoreGive(s4408736_SemaphoreRadioGetRxAddr);

	return pdFALSE;
}

/**
  * @brief  gettxaddr Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvGetTxAddrCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	xSemaphoreGive(s4408736_SemaphoreRadioGetTxAddr);

	return pdFALSE;
}

/**
  * @brief  join Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvJoinCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	xSemaphoreGive(s4408736_SemaphoreRadioJoin);

	return pdFALSE;
}

/**
  * @brief  Move Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvMoveCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString ){

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	char xyPosition[6];
	char xPosition[3];
	char yPosition[3];
	strcpy(xPosition, strtok(cCmd_string, " "));
	strcpy(yPosition, strtok(NULL, " "));

	//fill XY coordinates
	check_maximun_position(xPosition);
	check_maximun_position(yPosition);
	fill_xy_position(xPosition, yPosition, xyPosition);

	xQueueSendToFront(s4408736_QueueRadioPosition, ( void * ) &xyPosition,
			( portTickType ) 10 );

	return pdFALSE;
}

/**
  * @brief  Pen Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvPenCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ){

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	//Check if command is up or down
	if (!strcmp("up", cCmd_string)) {
		xSemaphoreGive(s4408736_SemaphoreRadioPenUp);
	} else if (!strcmp("down", cCmd_string)) {
		xSemaphoreGive(s4408736_SemaphoreRadioPenDown);
	}

	return pdFALSE;
}

/**
  * @brief Set height Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvSetHeightCommand(char *pcWriteBuffer,
		size_t xWriteBufferLen, const char *pcCommandString )  {

	long lParam_len;
	const char *cCmd_string;
	uint8_t height[2];

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	height[0] = cCmd_string[0];
	height[1] = cCmd_string[1];

	xQueueSendToFront(s4408736_QueueRadioSetHeight, ( void * ) &height,
			( portTickType ) 10 );

	return pdFALSE;
}

/**
  * @brief Origin Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvOriginCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString )  {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	xSemaphoreGive(s4408736_SemaphoreRadioOrigin);

	return pdFALSE;
}

/**
  * @brief Line Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvLineCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ){

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	char xyPosition[6];
	char xPosition[3];
	char yPosition[3];
	char type[1];
	char length[3];
	int result = 0;

	//Storing parameters into arrays
	strcpy(xPosition, strtok(cCmd_string, " "));
	strcpy(yPosition, strtok(NULL, " "));
	strcpy(type, strtok(NULL, " "));
	strcpy(length, strtok(NULL, " "));

	pen_up_down(0); // put pen down before drawing

	//generate coordinates
	for (int j = 0; j < 2; j++) {

		//send coordinates to plotter
		fill_xy_position(xPosition, yPosition, xyPosition);
		xQueueSendToBack(s4408736_QueueRadioGraphics, ( void * ) &xyPosition,
				( portTickType ) 10 );

		//vertical line
		if (type[0] == 'v') {

			result = (atoi(yPosition) + atoi(length));

			//check for out of bound coordinates
			if (result >= 200){
				result = (atoi(yPosition) - atoi(length));
			}

			if (result <= 0) {
				result = 0;
			}

			sprintf(yPosition,"%d",result);
			format_packet(result, yPosition);

		//Horizontal line
		} else if (type[0] == 'h') {

			result = (atoi(xPosition) + atoi(length));

			//check for out of bound coordinates
			if (result >= 200){
				result = (atoi(xPosition) - atoi(length));
			}

			if (result <= 0) {
				result = 0;
			}

			sprintf(xPosition,"%d" ,result);
			format_packet(result, xPosition);
		}
	}

	return pdFALSE;
}

/**
  * @brief Square Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvSquareCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	char xyPosition[6];
	char xPosition[3];
	char yPosition[3];
	int type = 0;
	char length[3];
	int result = 0;
	int ydirection = 1;
	int  xdirection = 1;

	//Storing parameters into arrays
	strcpy(xPosition, strtok(cCmd_string, " "));
	strcpy(yPosition, strtok(NULL, " "));
	strcpy(length, strtok(NULL, " "));

	pen_up_down(0); // put pen down before drawing

	//generate coordinates
	for (int j = 0; j < 5; j++) {

		//send coordinates to plotter
		fill_xy_position(xPosition, yPosition, xyPosition);
		xQueueSendToBack(s4408736_QueueRadioGraphics, ( void * ) &xyPosition,
				( portTickType ) 10 );

		//VERTICAL side
		if (type == 1) {

			//up or down
			if (ydirection) {

				result = (atoi(yPosition) + atoi(length));
				ydirection = 0;

			} else {

				result = (atoi(yPosition) - atoi(length));
				ydirection = 1;

			}

			//Check out of bounds
			if (result > 200){

				result = (atoi(yPosition) - atoi(length));
				ydirection = 1;

			}

			if (result < 0) {
				result = 0;
			}

			sprintf(yPosition,"%d",result);
			format_packet(result, yPosition);
			type = 0;

		//HORIZONTAL side
		} else {

			//Check for left or right
			if (xdirection == 1) {

				result = (atoi(xPosition) + atoi(length));
				xdirection = 0;

			} else {

				result = (atoi(xPosition) - atoi(length));
				xdirection = 1;
			}

			//Check out of bounds
			if (result > 200){

				result = (atoi(xPosition) - atoi(length));
				xdirection = 1;
			}

			if (result < 0) {
				result = 0;
			}

			sprintf(xPosition,"%d" ,result);
			format_packet(result, xPosition);
			type = 1;
		}
	}
	return pdFALSE;
}

/**
  * @brief BLine Command.
  * @param  writebuffer, writebuffer length and command strength
  * @retval None
  */
static BaseType_t prvBlineCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
		const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;
	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	int x0coordinate,x1coordinate,y0coordinate,y1coordinate,step;
	char x1Position[3];
	char y1Position[3];
	char x2Position[3];
	char y2Position[3];
	char stepSize[3];

	//Storing parameters into arrays
	strcpy(x1Position, strtok(cCmd_string, " "));
	strcpy(y1Position, strtok(NULL, " "));
	strcpy(x2Position, strtok(NULL, " "));
	strcpy(y2Position, strtok(NULL, " "));
	strcpy(stepSize, strtok(NULL, " "));


	//Store arguments into integers
	x0coordinate = atoi(x1Position);
	x1coordinate = atoi(x2Position);
	y0coordinate = atoi(y1Position);
	y1coordinate = atoi(y2Position);
	step = atoi(stepSize);

	pen_up_down(0); // put pen down before drawing
	generate_bline_coordinates(x0coordinate, x1coordinate, y0coordinate,
			y1coordinate, step);

	return pdFALSE;
}
