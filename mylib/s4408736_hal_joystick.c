/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_joystick.c
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   Joystick driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * extern void s4408736_hal_joystick_init(void) – intialise Joystick and ADC
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_joystick.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  Initialise joystick adn ADC
  * @param  None
  * @retval None
  */
extern void s4408736_hal_joystick_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	__BRD_A0_GPIO_CLK();
	__BRD_A1_GPIO_CLK();

	GPIO_InitStructure.Pin = BRD_A0_PIN;	//X
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = BRD_A1_PIN;	//Y
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BRD_A1_GPIO_PORT, &GPIO_InitStructure);

	__ADC1_CLK_ENABLE();

	AdcHandle.Instance = (ADC_TypeDef *)(ADC1_BASE);				//Use ADC1
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;
	AdcHandle.Init.ScanConvMode          = DISABLE;
	AdcHandle.Init.ContinuousConvMode    = DISABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.NbrOfDiscConversion   = 0;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion       = 1;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;
	AdcHandle.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandle);		//Initialise ADC

	/* Configure ADC Channel X*/
	AdcChanConfig.Channel = BRD_A0_ADC_CHAN;		//Use A0 pin
	AdcChanConfig.Rank         = 1;
	AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfig.Offset       = 0;

	// Configure ADC Channel Y
	AdcChanConfig1.Channel = BRD_A1_ADC_CHAN;		//Use A1 pin
	AdcChanConfig1.Rank         = 1;
	AdcChanConfig1.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfig1.Offset       = 0;
}

unsigned int joystick_read(ADC_ChannelConfTypeDef channel){

	unsigned int joystickValue = 0;
	HAL_ADC_ConfigChannel(&AdcHandle, &channel); //Initialise ADC channel
	HAL_ADC_Start(&AdcHandle); // Start ADC conversion

	// Wait for ADC conversion to finish
	while (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK);
	joystickValue = (uint16_t)(HAL_ADC_GetValue(&AdcHandle));

	return joystickValue;
}
