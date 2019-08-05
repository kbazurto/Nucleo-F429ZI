/**
 ******************************************************************************
 * @file    ex13_radio/main.c
 * @author  MDS
 * @date    21032016
 * @brief   Receive 32 byte packets using the radio FSM driver
 *			 See the nrf9051plus datasheet.
 ******************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IDLE_STATE 		0
#define TX_STATE		1
#define RX_STATE		2
#define WAITING_STATE 	3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t packetbuffer[32];
/* Private function prototypes -----------------------------------------------*/
void Hardware_init();

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
void main(void) {

	int i;
	int current_state = IDLE_STATE; // Current state of FSM
	uint8_t current_channel;
	uint8_t new_channel = 46;
	BRD_init();	//Initialise NP2
	Hardware_init();	//Initialise hardware modules


	// Initialise radio FSM
	radio_fsm_init();

	// set radio FSM state to IDLE
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
	//radio_fsm_register_write(NRF24L01P_RF_CH, 46);
	/* Main processing loop */
	while (1) {

		// Receiving FSM
		switch (current_state) {

		case IDLE_STATE:

			// Get current channel, if radio FSM is in IDLE state
			if (radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) {


				radio_fsm_register_read(NRF24L01P_RF_CH, &current_channel); // Read channel
				//current_channel = BRD_debuguart_getc();

				debug_printf("Current channel %X\n\r", current_channel);

				//current_state = RX_STATE; // Set next state as RX state
				current_state = TX_STATE;
			} else {
				// if error occurs set state back to IDLE state
				debug_printf("ERROR: Radio FSM not in IDLE state\r\n");
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);

			}

			break;

		case TX_STATE:
			radio_fsm_setstate(RADIO_FSM_TX_STATE);
			HAL_Delay(100);
			radio_fsm_register_write(NRF24L01P_RF_CH, &new_channel);
			current_state = RX_STATE;
			radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			HAL_Delay(100);
			break;

		case RX_STATE: // RX state for putting radio transceiver into receive mode

			// Put radio in RX state if radio FSM is in IDLE or in waiting state
			if ((radio_fsm_getstate() == RADIO_FSM_IDLE_STATE)
					|| (radio_fsm_getstate() == RADIO_FSM_WAIT_STATE)) {

				if (radio_fsm_setstate(RADIO_FSM_RX_STATE)
						== RADIO_FSM_INVALIDSTATE) {
					debug_printf("ERROR: Cannot set radio FSM to RX state\r\n");
					HAL_Delay(100);
				} else {

					current_state = WAITING_STATE;
				}

			} else {

				// if error occurs set state back to IDLE state
				debug_printf("ERROR: Radio FSM no in IDLE state\r\n");
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}

			break;

		case WAITING_STATE: // Waiting state for reading received packet

			// Check if radio FSM is in WAITING state
			if (radio_fsm_getstate() == RADIO_FSM_WAIT_STATE) {

				// Check for received packet and display
				if (radio_fsm_read(packetbuffer) == RADIO_FSM_DONE) {

					debug_printf("Received: ");
					for (i = 0; i < 32; i++) {
						debug_printf("%x ", packetbuffer[i]);
					}
					debug_printf("\r\n");

				}

				radio_fsm_setstate(RADIO_FSM_IDLE_STATE); // Set radio FSM back to IDLE state

				current_state = IDLE_STATE;
			}

			break;
		}

		BRD_LEDBlueToggle();	//Toggle LED on/off
		HAL_Delay(100);	//Delay function

	}

}

/**
 * @brief  Initialise Hardware
 * @param  None
 * @retval None
 */
void Hardware_init() {

	BRD_LEDInit();		//Initialise Blue LED
	/* Turn off LEDs */
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();

	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


}

