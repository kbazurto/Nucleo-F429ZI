/**
 ******************************************************************************
 * @file    mylib/s4408736_os_APCRadio.c
 * @author  Kevin Bazurto – 44087368
 * @date    20/05/2018
 * @brief   Task to control radio
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * extern void s4408736_os_apcradio_init(void) - initialize task and semaphores
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

#include "s4408736_hal_hamming.h"
#include "s4408736_os_printf.h"
#include "s4408736_os_APCRadio.h"
#include "s4408736_hal_radio.h"
#include "s4408736_os_APCDisplay.h"

/* Private define ------------------------------------------------------------*/
#define ACKRECEIVED 1
#define NOACK 0
#define XYZ 0
#define XY 6
#define Z 18

/* Private variables ---------------------------------------------------------*/

int receive = ACKRECEIVED; //flag for ACK

//variables to communicate with plotter
char setChannel;

uint8_t rxAddr[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t txAddr[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t xyPosition[6] = {'0','0','0','0','0','0'};
uint8_t zPosition[2] = {'0','0'};
uint8_t newZPosition[2] = {'4','0'}; ///////////
uint8_t xyz[3] = {'X','Y','Z'};
uint8_t joinWord[4] = {'J', 'O', 'I', 'N'};
uint8_t ackWord[5] = {'A', ' ', 'C', ' ','K'};

uint8_t joinEncoded[22] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00};

uint8_t completePacket[32] = {0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char s4408736_rx_packet[32];
unsigned char rx_packet_decoded[5];

/* Task Priorities -----------------------------------------------------------*/
#define RADIOTASK_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define RECEIVERTASK_PRIORITY					( tskIDLE_PRIORITY + 3 )
#define FSMTASK_PRIORITY					( tskIDLE_PRIORITY + 4 )

/* Task Stack Allocations ----------------------------------------------------*/
#define RADIOTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define FSMTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define RECEIVERTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

/* Private function prototypes -----------------------------------------------*/
void s4408736_TaskRadio( void );
void s4408736_TaskFsm( void );
void s4408736_TaskRadioReceiver( void );

/**
  * @brief  Create tasks, queues and semaphores
  * @param  None
  * @retval None
  */
extern void s4408736_os_apcradio_init(void) {

	/*initialize tasks*/
	xTaskCreate( (void *) &s4408736_TaskRadio, (const signed char *) "TAKE",
			RADIOTASK_STACK_SIZE, NULL, RADIOTASK_PRIORITY, NULL );

	xTaskCreate( (void *) &s4408736_TaskFsm, (const signed char *) "FSM",
			FSMTASK_STACK_SIZE, NULL, FSMTASK_PRIORITY, NULL );

	xTaskCreate( (void *) &s4408736_TaskRadioReceiver,
			(const signed char *) "RECEIVE", RECEIVERTASK_STACK_SIZE, NULL,
			RECEIVERTASK_PRIORITY, NULL );

	/*initialize semaphores*/

	s4408736_SemaphoreRadioGetSys = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioGetChan = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioGetRxAddr = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioGetTxAddr = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioJoin = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioPenUp = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioPenDown = xSemaphoreCreateBinary();
	s4408736_SemaphoreRadioOrigin = xSemaphoreCreateBinary();

	/*initialize Queues*/

	s4408736_QueueRadioChannel = xQueueCreate(10, sizeof(setChannel));
	s4408736_QueueRadioRxAddr = xQueueCreate(10, sizeof(rxAddr));
	s4408736_QueueRadioTxAddr = xQueueCreate(10, sizeof(txAddr));
	s4408736_QueueRadioPosition = xQueueCreate(10, sizeof(xyPosition));
	s4408736_QueueRadioSetHeight = xQueueCreate(10, sizeof(newZPosition));
	s4408736_QueueRadioGraphics = xQueueCreate(200, sizeof(xyPosition));
}

/**
  * @brief  Clear transmit buffer
  * @param  None
  * @retval None
  */
void clear_buffer(void) {

	for (int j = 0; j < 32; j++) {
		s4408736_rx_packet[j] = 0;
	}
	for (int j = 0; j < 5; j++){
		rx_packet_decoded[j] = 0;
	}
}

/**
  * @brief  Fill transmit buffer
  * @param  None
  * @retval None
  */
void generate_packet(uint8_t *tx, uint8_t *rx, uint8_t *payload) {

	for (int i = 0; i < 32; i++) {

		if (i < 4) {
			completePacket[i+1] = tx[i]; //tx addr
		} else if (i >= 4 && i < 8) {
			completePacket[i + 1] = rx[i - 4]; //rx addr
		} else if (i >= 9) {
			completePacket[i + 1] = payload[i - 9]; //payload
		}

	}
}

/**
  * @brief  Encode payload and fill buffer
  * @param  None
  * @retval None
  */
void fill_tx_buffer_section(uint8_t *tx_buffer, uint8_t *packet, int index,
		int size){

	int k = index; //indew = where to start filling

	for (int j = 0; j < size; j++) {

		tx_buffer[k] = s4408736_hal_hamming_encoder(packet[j] >> 4);
		tx_buffer[k + 1] = s4408736_hal_hamming_encoder(packet[j]);
		k = k + 2;

	}
}

/**
  * @brief  Fill encode xyz buffer with correct format
  * @param  None
  * @retval None
  */
void fill_xyz(void){

	fill_tx_buffer_section(joinEncoded, xyz, XYZ,
			(int)( sizeof(xyz) / sizeof(xyz[0]) ));

	fill_tx_buffer_section(joinEncoded, xyPosition, XY,
			(int)( sizeof(xyPosition) / sizeof(xyPosition[0]) ));

	fill_tx_buffer_section(joinEncoded, zPosition, Z,
			(int)( sizeof(zPosition) / sizeof(zPosition[0]) ));

}

/**
  * @brief  Clear xyPosition buffer and zPosition buffer
  * @param  None
  * @retval None
  */
void clear_position(void){

	for (int i = 0; i < 6; i++) {

		if (i >= 0 && i < 2) {
			zPosition[i] = '0';
		}
		xyPosition[i] = '0';

	}
}

/**
  * @brief  Clear transmit buffer
  * @param  None
  * @retval None
  */
void clear_packet(void){

	for (int i = 0; i < 22; i++) {
		joinEncoded[i] = 0x00;
	}
}

/**
  * @brief  Set Z axis position
  * @param  Direction (1) for up, (0) for down
  * @retval None
  */
extern void pen_up_down (int direction) {

	if (direction == 1) {
		zPosition[0] = '0';
		zPosition[1] = '0';
	} else {
		zPosition[0] = newZPosition[0];
		zPosition[1] = newZPosition[1];
	}
}

/**
  * @brief  Format packet, encoded and send it
  * @param  Direction (1) for up, (0) for down
  * @retval None
  */
void send_coordinates(void){

	fill_xyz();
	generate_packet(txAddr,rxAddr,joinEncoded);

	s4408736_hal_radio_sendpacket(setChannel, txAddr, completePacket);

	//Pan tilt
	xQueueSendToFront(s4408736_QueuePanTiltMove, ( void * ) &xyPosition,
				( portTickType ) 10 );

}


/**
  * @brief  Take Task. Used to take semaphore events
  * @param  None
  * @retval None
  */
void s4408736_TaskRadio( void ) {

	for (;;) {

		//Get system running time
		if (s4408736_SemaphoreRadioGetSys != NULL) {

			if (xSemaphoreTake( s4408736_SemaphoreRadioGetSys, 10 ) == pdTRUE) {
				myprintf("System time: %d\r\n", xTaskGetTickCount());
			}
		}

		//print current radio channel
		if (s4408736_SemaphoreRadioGetChan != NULL) {

			if (xSemaphoreTake( s4408736_SemaphoreRadioGetChan, 10) == pdTRUE) {
				myprintf("Channel %d\r\n", s4408736_hal_radio_getchan());
			}
		}

		//print current Rx address
		if (s4408736_SemaphoreRadioGetRxAddr != NULL) {

			if ( xSemaphoreTake( s4408736_SemaphoreRadioGetRxAddr, 10 ) ==
					pdTRUE ) {

				unsigned char rxAddress[5];
				s4408736_hal_radio_getrxaddress(rxAddress);
				myprintf("RxAddr: %02x%02x%02x%02x\r\n", rxAddress[0],
						rxAddress[1], rxAddress[2], rxAddress[3]);

			}
		}

		//Printf current Tx address
		if (s4408736_SemaphoreRadioGetTxAddr != NULL) {

			if ( xSemaphoreTake( s4408736_SemaphoreRadioGetTxAddr, 10 ) ==
					pdTRUE ) {

				unsigned char txAddress[5];
				s4408736_hal_radio_gettxaddress(txAddress);
				myprintf("TxAddr: %02x%02x%02x%02x\r\n", txAddress[0],
						txAddress[1], txAddress[2], txAddress[3]);

			}
		}

		//Send join packet
		if (s4408736_SemaphoreRadioJoin != NULL) {

			if ( xSemaphoreTake( s4408736_SemaphoreRadioJoin, 10 ) == pdTRUE ) {

				clear_packet(); //fill pakcet with 0 before sending join

				fill_tx_buffer_section(joinEncoded, joinWord, 0,
						(int)( sizeof(joinWord) / sizeof(joinWord[0]) ));

				generate_packet(txAddr,rxAddr,joinEncoded);

				s4408736_hal_radio_sendpacket(setChannel, txAddr,
						completePacket);

			}
		}

		//set radio channel
		if (xQueueReceive( s4408736_QueueRadioChannel, &setChannel, 10 )) {
			s4408736_hal_radio_setchan(setChannel);
    	}

		// set radio Rx address
		if (xQueueReceive( s4408736_QueueRadioRxAddr, &rxAddr, 10 )) {
			s4408736_hal_radio_setrxaddress(rxAddr);
    	}

		//set radio Tx address
		if (xQueueReceive( s4408736_QueueRadioTxAddr, &txAddr, 10 )) {
			s4408736_hal_radio_settxaddress(txAddr);
    	}

		//Move plotter
		if (xQueueReceive( s4408736_QueueRadioPosition, &xyPosition, 10 )) {
			send_coordinates();
    	}

		//Move Z axis to 0
		if (s4408736_SemaphoreRadioPenUp != NULL) {

			if (xSemaphoreTake( s4408736_SemaphoreRadioPenUp, 10 ) == pdTRUE ) {
				pen_up_down(1);
				send_coordinates();
			}
		}

		//Move Z axis to white board height
		if (s4408736_SemaphoreRadioPenDown != NULL) {

			if ( xSemaphoreTake( s4408736_SemaphoreRadioPenDown, 10 ) ==
					pdTRUE ) {

				pen_up_down(0);
				send_coordinates();
			}
		}

		//Set Z axis height
		if (xQueueReceive( s4408736_QueueRadioSetHeight, &newZPosition, 10 )) {
			myprintf("Plotter height set to %c%c\r\n", newZPosition[0],
					newZPosition[1]);

    	}

		//Move plotter pen to origin
		if (s4408736_SemaphoreRadioOrigin != NULL) {

			if (xSemaphoreTake( s4408736_SemaphoreRadioOrigin, 10 ) == pdTRUE) {
				clear_position();
				send_coordinates();
			}
		}

		//Send packets to draw a shape
		if (receive) {

			if (xQueueReceive( s4408736_QueueRadioGraphics, &xyPosition, 10 )) {
				send_coordinates();
				receive = NOACK;

			}
		}
		vTaskDelay(1000);
	}
}

/**
  * @brief  Task to receive packets through radio and wait for ACK
  * @param  None
  * @retval None
  */
void s4408736_TaskRadioReceiver( void ) {

	for (;;) {

		//Receive
		s4408736_hal_radio_set_fsmrx();
		if(s4408736_hal_radio_getrxstatus() == 1){

			s4408736_hal_radio_getpacket(s4408736_rx_packet);
			int i = 0;

			for(int k = 10; k < 19; k = k + 2){

				//get ACK
				rx_packet_decoded[i] = s4408736_decode(s4408736_rx_packet[k],
						s4408736_rx_packet[k+1]);

				i++;
			}

			receive = ACKRECEIVED; //set flag up

			myprintf("Received from plotter: %c %c %c %c %c \n\r",
					rx_packet_decoded[0], rx_packet_decoded[1],
					rx_packet_decoded[2], rx_packet_decoded[3],
					rx_packet_decoded[4]);

		}
		clear_buffer();
		vTaskDelay(2000);
	}
}

/**
  * @brief  Task to run fsm processing every 100 ms
  * @param  None
  * @retval None
  */
void s4408736_TaskFsm( void ) {

	for (;;) {

		s4408736_hal_radio_fsmprocessing();
		vTaskDelay(100);
	}
}
