/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_ledbar.c
 * @author  Kevin Bazurto – 44087368
 * @date    01/03/2018
 * @brief   LED Light Bar peripheral driver
 *	     REFERENCE: LEDLightBar_datasheet.pdf
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_ledbar_init() – intialise LED Light BAR
 * s4408736_hal_ledbar_write() – set LED Light BAR value
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_ledbar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define ON 1
#define OFF 0
#define LEDNUMBER 10
/* Private variables ---------------------------------------------------------*/


/**
  * @brief  Initialise LEDBar.
  * @param  None
  * @retval None
  */
extern void s4408736_hal_ledbar_init(void) {
	// Configure the GPIO_D16 pin to GPIO_D25 pin
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Enable the D16...D25 Clock */
  	__BRD_D16_GPIO_CLK();
	__BRD_D17_GPIO_CLK();
	__BRD_D18_GPIO_CLK();
	__BRD_D19_GPIO_CLK();
	__BRD_D20_GPIO_CLK();
	__BRD_D21_GPIO_CLK();
	__BRD_D22_GPIO_CLK();
	__BRD_D23_GPIO_CLK();
	__BRD_D24_GPIO_CLK();
	__BRD_D25_GPIO_CLK();

  	/* Configure the D16 pin as an output */
	GPIO_InitStructure.Pin = BRD_D16_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D16_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	/* Configure the D17 pin as an output */
	GPIO_InitStructure.Pin = BRD_D17_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;			//output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D17_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D18 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D18_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D18_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D19 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D19_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;			//output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D19_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D20 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D20_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D20_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D21 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D21_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;			//output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D21_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D22 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D22_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D22_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D23 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D23_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;			//output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D23_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D24 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D24_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D24_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

  	/* Configure the D25 pin as an output */
  	GPIO_InitStructure.Pin = BRD_D25_PIN;				//Pin
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  	HAL_GPIO_Init(BRD_D25_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin
}

/**
 * Turn on or off LEDs according to the given parameters
 */
void ledbar_seg_set(int segment, unsigned char segment_value){
	if (segment == 0)	{//Turn on LED BAR Segment 0.
		HAL_GPIO_WritePin(BRD_D16_GPIO_PORT, BRD_D16_PIN, segment_value & 0x01);
	} else if (segment == 1)	{//Turn on LED BAR Segment 1.
		HAL_GPIO_WritePin(BRD_D17_GPIO_PORT, BRD_D17_PIN, segment_value & 0x01);
	} else if (segment == 2)	{//Turn on LED BAR Segment 2.
		HAL_GPIO_WritePin(BRD_D18_GPIO_PORT, BRD_D18_PIN, segment_value & 0x01);
	} else if (segment == 3)	{//Turn on LED BAR Segment 3.
		HAL_GPIO_WritePin(BRD_D19_GPIO_PORT, BRD_D19_PIN, segment_value & 0x01);
	} else if (segment == 4)	{//Turn on LED BAR Segment 4.
		HAL_GPIO_WritePin(BRD_D20_GPIO_PORT, BRD_D20_PIN, segment_value & 0x01);
	} else	if (segment == 5)	{//Turn on LED BAR Segment 5.
		HAL_GPIO_WritePin(BRD_D21_GPIO_PORT, BRD_D21_PIN, segment_value & 0x01);
	} else	if (segment == 6)	{//Turn on LED BAR Segment 6.
		HAL_GPIO_WritePin(BRD_D22_GPIO_PORT, BRD_D22_PIN, segment_value & 0x01);
	} else	if (segment == 7)	{//Turn on LED BAR Segment 7.
		HAL_GPIO_WritePin(BRD_D23_GPIO_PORT, BRD_D23_PIN, segment_value & 0x01);
	} else	if (segment == 8)	{//Turn on LED BAR Segment 8.
		HAL_GPIO_WritePin(BRD_D24_GPIO_PORT, BRD_D24_PIN, segment_value & 0x01);
	} else if (segment == 9) {//Turn on LED BAR Segment 9.
		HAL_GPIO_WritePin(BRD_D25_GPIO_PORT, BRD_D25_PIN, segment_value & 0x01);
	}
}

/**
  * @brief  Set the LED Bar GPIO pins high or low
  *         i.e. value bit 0 is 1 – LED Bar 0 on
  *          value bit 1 is 1 – LED BAR 1 on
  * @param  value
  * @retval None
  */
extern void s4408736_hal_ledbar_write(unsigned short value) {
	/* Use bit shifts and bit masks to determine if a bit is set
	   e.g. The following pseudo code checks if bit 0 of value is 1.
	*/
	for(int LEDPosition = 0; LEDPosition < LEDNUMBER; LEDPosition++){
		if ((value & (1 << LEDPosition)) == (1 << LEDPosition)) {
			ledbar_seg_set(LEDPosition, ON);
		} else {
			ledbar_seg_set(LEDPosition, OFF);
		}
	}
}

