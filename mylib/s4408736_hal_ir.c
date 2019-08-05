/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_ir.c
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   IR driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_ir_init(void) – intialise IR receiver and transmmiter
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_ir.h"

/* Private define ------------------------------------------------------------*/
#define PWM_CLOCKFREQ	1600000 //1.6MHz
#define PWM_FREQUENCY	38000
#define PWM_PERIOD		2*PWM_CLOCKFREQ/PWM_FREQUENCY
#define PULSE_WIDTH    (uint32_t)(PWM_PERIOD*50/100) //50% duty cycle
#define IR_TIMER		TIM2
#define CARRIER_PIN		BRD_D35_PIN
#define INPUT_PIN		BRD_D36_PIN
#define CARRIER_CHANNEL	TIM_CHANNEL_4
#define INPUT_CHANNEL	TIM_CHANNEL_5
#define CARRIER_PORT	BRD_D35_GPIO_PORT

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef TIM_Init;
TIM_HandleTypeDef TIM4_Init;
TIM_OC_InitTypeDef PWMConfig;

/**
  * @brief  Initialise IR
  * @param  None
  * @retval None
  */
extern void s4408736_hal_ir_init(){

	//variables for TIMERs
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_IC_InitTypeDef  TIM_ICInitStructure;
	uint16_t PrescalerValue = 0;

	__TIM2_CLK_ENABLE();
	__TIM4_CLK_ENABLE();

	//Initialize pins
	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / PWM_CLOCKFREQ) - 1;

	__BRD_D10_GPIO_CLK();
	// Configure the D10 pin with TIM3 input capture
	GPIO_InitStructure.Pin = BRD_D10_PIN;				//Pin
	GPIO_InitStructure.Mode =GPIO_MODE_AF_PP; //Set mode to be output alternate
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
	GPIO_InitStructure.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(BRD_D10_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin
	//--------------------------------------------
	// Configure Timer 4 settings
	TIM4_Init.Instance = TIM4;					//Enable Timer 4
	TIM4_Init.Init.Period = 2*50000/10;			//Set for (10 Hz) period
	TIM4_Init.Init.Prescaler = PrescalerValue;	//Set presale value
	TIM4_Init.Init.ClockDivision = 0;			//Set clock division
	TIM4_Init.Init.RepetitionCounter = 0; 		// Set Reload Value
	TIM4_Init.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up
	//----------------------------------------------
	// Configure TIM2 Input capture
	//Set to trigger on rising/falling edge
	TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
	TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.ICFilter = 0;

	// Set priority of Timer 4 Interrupt [0 (HIGH priority) to 15(LOW priority)]
	HAL_NVIC_SetPriority(TIM4_IRQn, 10, 0);
	// Enable Timer 4 interrupt and interrupt vector
	NVIC_EnableIRQ(TIM4_IRQn);
	// Enable input capture for Timer 2, channel 3
	HAL_TIM_IC_Init(&TIM4_Init);
	HAL_TIM_IC_ConfigChannel(&TIM4_Init, &TIM_ICInitStructure, TIM_CHANNEL_3);
	//Start Input Capture
	HAL_TIM_IC_Start_IT(&TIM4_Init, TIM_CHANNEL_3);

	__BRD_D35_GPIO_CLK();
	__BRD_D38_GPIO_CLK();

	/* Configure the D0 pin as an output */
	GPIO_InitStructure.Pin = BRD_D38_PIN;				//Pin
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
	HAL_GPIO_Init(BRD_D38_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = CARRIER_PIN;
	GPIO_InitStructure.Mode =GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(CARRIER_PORT, &GPIO_InitStructure);

	//Set timer
	TIM_Init.Instance = IR_TIMER;
	TIM_Init.Init.Period = PWM_PERIOD;
	TIM_Init.Init.Prescaler = PrescalerValue;
	TIM_Init.Init.ClockDivision = 0;
	TIM_Init.Init.RepetitionCounter = 0;
	TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

	//Set PMW
	PWMConfig.OCMode = TIM_OCMODE_PWM1;
	PWMConfig.Pulse = PULSE_WIDTH;
	PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
	PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
	PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	HAL_TIM_PWM_Init(&TIM_Init);
	HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, CARRIER_CHANNEL);
	HAL_TIM_PWM_Start(&TIM_Init, CARRIER_CHANNEL);

}

/**
  * @brief  Turn off ad on carrier wave
  * @param  state : 0 = off, 1 = On
  * @retval None
  */
void irhal_carrier(int state){
	if (state == 0) {
		HAL_TIM_PWM_Stop(&TIM_Init, CARRIER_CHANNEL);
	} else {
		HAL_TIM_PWM_Start(&TIM_Init, CARRIER_CHANNEL);
	}
}

