/**
  ******************************************************************************
  * @file    demo4/main.c
  * @author  Kevin Bazurto 44087368
  * @date    22/03/2018
  * @brief   Radio control
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_radio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef TIM_Init1;
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);
void TIM2_IRQHandler(void);


unsigned char packet_word[] = {0x20, 0x53, 0x33, 0x22, 0x11, 0x68, 0x73, 0x08, 0x44, 'k', 'e', 'v', 'i', 'n', 'b', 'a'};
uint8_t address[] = {0x53, 0x33, 0x22, 0x11, 0x00};
char channel = 53;

unsigned char s4408736_tx_packet[32] = {0x20, 0x53, 0x33, 0x22, 0x11, 0x68, 0x73, 0x08, 0x44, 'k', 'e', 'v', 'i', 'n', 'b', 'a'};
unsigned char TESTpacket[32] = {0xA1, 0x53, 0x33, 0x22, 0x11, 0x68, 0x73, 0x08, 0x44, 0x00, 0x47, 0xA6, 0x47, 0xFF, 0x47, 0x93, 0x47, 0xE1, 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char s4408736_rx_packet[32];
uint8_t base_address[] = {0x68, 0x73, 0x08, 0x44, 0x00};
unsigned char add[32];
unsigned char add2[32];

/**
  * @brief  Main program - Drives Radio
  * @param  None
  * @retval None
  */
void main(void)  {
	BRD_init();
	Hardware_init();

	s4408736_hal_radio_setrxaddress(base_address);
	s4408736_hal_radio_setchan(channel);

	while (1) {
		debug_printf("here\n\r");
		/*
		debug_printf("here\n\r");
		s4408736_hal_radio_gettxaddress(add);
			s4408736_hal_radio_getrxaddress(add2);
			debug_printf ("channel %d, rx %x%x%x%x, tx %x%x%x%x\r\n", s4408736_hal_radio_getchan(), add2[0], add2[1], add2[2], add2[3], add[0], add[1], add[2], add[3]);
		*/
		HAL_Delay(5000);
		//s4408736_hal_radio_sendpacket(channel, address, packet_word);
		s4408736_hal_radio_sendpacket(channel, address, TESTpacket);
//		s4408736_hal_radio_getrxaddress(add);
//		debug_printf("COMINEZOOO\r\n");
//		debug_printf("%x\r\n", add[0]);
//		debug_printf("%x\r\n", add[1]);
//		debug_printf("%x\r\n", add[2]);
//		debug_printf("%x\r\n", add[3]);
//		debug_printf("%x\r\n", add[4]);
		HAL_Delay(100);
		s4408736_hal_radio_set_fsmrx();
		if(s4408736_hal_radio_getrxstatus() == 1){
			s4408736_hal_radio_getpacket(s4408736_rx_packet);
			debug_printf("RECIBIDO \r\n");
			for(int k =0; k < 16; k++){
				debug_printf("%x %c \r\n", s4408736_rx_packet[k],s4408736_rx_packet[k]);
			}
		}

    }
}


/**
 * @brief  Initialise Hardware
 * @param  None
 * @retval None
 */
void Hardware_init(void) {
	s4408736_hal_radio_init();
	unsigned short PrescalerValue;
	// Timer 2 clock enable
	__TIM2_CLK_ENABLE();

	// Compute the prescaler value
	// Set the clock prescaler to 50kHz
	// SystemCoreClock is the system clock frequency
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

	/* TIM Base configuration */
	TIM_Init1.Instance = TIM2;				//Enable Timer 2
	TIM_Init1.Init.Period = 100*(50000/1000);			//Set period count to be 1ms, so timer interrupt occurs every 1ms.
	TIM_Init1.Init.Prescaler = PrescalerValue;	//Set prescaler value
	TIM_Init1.Init.ClockDivision = 0;			//Set clock division
	TIM_Init1.Init.RepetitionCounter = 0;	// Set reload Value
	TIM_Init1.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up.

	/* Initialise Timer */
	HAL_TIM_Base_Init(&TIM_Init1);
	/* Set priority of Timer 2 update Interrupt [0 (HIGH priority) to 15(LOW priority)] */
	/* 	DO NOT SET INTERRUPT PRIORITY HIGHER THAN 3 */
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);		//Set Main priority to 10 and sub-priority to 0.
	// Enable the Timer 2 interrupt
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	// Start Timer 2 base unit in interrupt mode
	HAL_TIM_Base_Start_IT(&TIM_Init1);
}

/**
 * @brief Period elapsed callback in non blocking mode
 * @param htim: Pointer to a TIM_HandleTypeDef that contains the configuration information for the TIM module.
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	s4408736_hal_radio_fsmprocessing();
}

//Override default mapping of this handler to Default_Handler
void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&TIM_Init1);
}

