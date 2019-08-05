/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_sysmon.c
 * @author  Kevin Bazurto – 44087368
 * @date    02/05/2018
 * @brief   handle pins
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_sysmon_init(void) – intialise pins d10 d11 d12
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_sysmon.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Initialise Pins
  * @param  None
  * @retval None
  */
extern void s4408736_hal_sysmon_init(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the D0 & D1 Clock */
	__BRD_D10_GPIO_CLK();
	__BRD_D11_GPIO_CLK();
	__BRD_D12_GPIO_CLK();

	/* Configure the D10 pin as an output */
	GPIO_InitStructure.Pin = BRD_D10_PIN;				//Pin
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;			//Enable Pull up, down or no pull resister
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
	HAL_GPIO_Init(BRD_D10_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	/* Configure the D10 pin as an output */
	GPIO_InitStructure.Pin = BRD_D11_PIN;				//Pin
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;			//Enable Pull up, down or no pull resister
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
	HAL_GPIO_Init(BRD_D11_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	/* Configure the D10 pin as an output */
	GPIO_InitStructure.Pin = BRD_D12_PIN;				//Pin
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;			//Enable Pull up, down or no pull resister
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
	HAL_GPIO_Init(BRD_D12_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin
}

