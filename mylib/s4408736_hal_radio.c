/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_radio.c
 * @author  Kevin Bazurto – 44087368
 * @date    21/03/2018
 * @brief   Radio driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *void s4408736_hal_radio_init();
 *void s4408736_hal_radio_init(void);
 *void s4408736_hal_radio_fsmprocessing(void);
 *void s4408736_hal_radio_setchan(unsigned char channel);
 *void s4408736_hal_radio_settxaddress(unsigned char *address);
 *void s4408736_hal_radio_setrxaddress(unsigned char *address);
 *unsigned char s4408736_hal_radio_getchan(void);
 *void s4408736_hal_radio_gettxaddress(unsigned char *address);
 *void s4408736_hal_radio_getrxaddress(unsigned char *address);
 *void s4408736_hal_radio_sendpacket(char channel, unsigned char *address, unsigned char *txpacket);
 *void s4408736_hal_radio_set_fsmrx(void);
 *void s4408736_hal_radio_getpacket(unsigned char *rxpacket);
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"
#include "string.h"
#include "s4408736_hal_radio.h"

/* Private define ------------------------------------------------------------*/
#define S4408736_IDLE_STATE 	0
#define S4408736_RX_STATE		1
#define S4408736_TX_STATE		2
#define S4408736_WAITING_STATE 	3

extern void s4408736_hal_radio_setrxaddress(unsigned char *address);

/* Private variables ---------------------------------------------------------*/

//Variables to control state machine
static int s4408736_hal_radio_fsmcurrentstate = S4408736_IDLE_STATE;
int s4408736_hal_radio_rxstatus;
unsigned char s4408736_rx_buffer[32];
uint8_t s4408736_tx_buffer[32];

/**
  * @brief  Initialise SPI pins
  * @param  None
  * @retval None
  */
extern void s4408736_hal_radio_init(void) {
	radio_fsm_init();
	radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
}


/**
  * @brief  Set channel of the radio
  * @param  channel: channel to be set
  * @retval None
  */
extern void s4408736_hal_radio_setchan(unsigned char channel) {
	radio_fsm_register_write(NRF24L01P_RF_CH, &channel);
}

/**
  * @brief  Set transmit address of the radio
  * @param  address: address to be set
  * @retval None
  */
extern void s4408736_hal_radio_settxaddress(unsigned char *address) {
	radio_fsm_buffer_write(NRF24L01P_TX_ADDR, address, 5);
}

/**
  * @brief  Set receive address of the radio
  * @param  address: address to be set
  * @retval None
  */
extern void s4408736_hal_radio_setrxaddress(unsigned char *address) {
	radio_fsm_buffer_write(NRF24L01P_RX_ADDR_P0, address, 5);
}

/**
  * @brief  Get current channel of radio
  * @param  None
  * @retval currentChannel
  */
extern unsigned char s4408736_hal_radio_getchan(void) {
	uint8_t currentChannel;
	radio_fsm_register_read(NRF24L01P_RF_CH, &currentChannel);
	return currentChannel;
}

/**
  * @brief  Get transmit address of the radio
  * @param  address: pointer to store address
  * @retval None
  */
extern void s4408736_hal_radio_gettxaddress(unsigned char *address) {
	radio_fsm_buffer_read(NRF24L01P_TX_ADDR, address, 5);
}

/**
  * @brief  Get receive address of the radio
  * @param  address: pointer to store address
  * @retval None
  */
extern void s4408736_hal_radio_getrxaddress(unsigned char *address) {
	radio_fsm_buffer_read(NRF24L01P_RX_ADDR_P0, address, 5);
}

/**
  * @brief  Send packet through radio
  * @param  channel : channel to be set
  * 		address: address to be set
  * 		txpacket: packet to be sent
  * @retval None
  */
extern void s4408736_hal_radio_sendpacket(char channel, unsigned char *address,
		unsigned char *txpacket) {

	s4408736_hal_radio_setchan(channel);
	s4408736_hal_radio_settxaddress(address);
	memcpy(s4408736_rx_buffer, txpacket, 32);
	s4408736_hal_radio_fsmcurrentstate = S4408736_TX_STATE;
}

/**
  * @brief  Set state machine to receive state
  * @param  None
  * @retval None
  */
extern void s4408736_hal_radio_set_fsmrx(void) {
	s4408736_hal_radio_fsmcurrentstate = S4408736_RX_STATE;
}

/**
  * @brief  Get status of packet
  * @param  None
  * @retval return 0 = no packet received or
  * 		1 = packet received
  */
extern int s4408736_hal_radio_getrxstatus(void) {
	return s4408736_hal_radio_rxstatus;
}

/**
  * @brief  Receive packet(only call function if
  * 		s4408736_hal_radio_getrxstatus == 1)
  * @param  rxpacket to store packet
  * @retval None
  */
extern void s4408736_hal_radio_getpacket(unsigned char *rxpacket) {

	if (s4408736_hal_radio_rxstatus == 1) {
		memcpy(rxpacket,s4408736_tx_buffer, 32);
		s4408736_hal_radio_rxstatus = 0;
	}
}

/**
  * @brief  Handles fsm to communicate with radio
  * @param  None
  * @retval None
  */
extern void s4408736_hal_radio_fsmprocessing(void) {

	switch (s4408736_hal_radio_fsmcurrentstate) {
		//IDLE STATE
		case S4408736_IDLE_STATE:

			if (radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) {
			} else {
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}
			break;

		//TX STATE
		case S4408736_TX_STATE:
			if (radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) {

				if (radio_fsm_setstate(RADIO_FSM_TX_STATE)
						== RADIO_FSM_INVALIDSTATE) {

				} else {

					//SEND PACKET
					radio_fsm_write(s4408736_rx_buffer);
					s4408736_hal_radio_fsmcurrentstate = S4408736_IDLE_STATE;
				}
			} else {
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}
			break;

		//RX STATE
		case S4408736_RX_STATE:

			if((radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) ||
					(radio_fsm_getstate() == RADIO_FSM_WAIT_STATE)){

				if (radio_fsm_setstate(RADIO_FSM_RX_STATE) ==
						RADIO_FSM_INVALIDSTATE) {

				} else {
					s4408736_hal_radio_fsmcurrentstate = S4408736_WAITING_STATE;
				}
			} else {
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
			}
			break;

		//WAITING STATE
		case S4408736_WAITING_STATE:

			if (radio_fsm_getstate() == RADIO_FSM_WAIT_STATE) {

				if (radio_fsm_read(s4408736_tx_buffer) == RADIO_FSM_DONE) {
					s4408736_hal_radio_rxstatus = 1;
				}
				radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
				s4408736_hal_radio_fsmcurrentstate = S4408736_IDLE_STATE;
			}
			break;
	}
}
