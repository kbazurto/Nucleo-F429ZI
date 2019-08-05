/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_pantilt.c
 * @author  Kevin Bazurto – 44087368
 * @date    04/03/2018
 * @brief   PanTilt device driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_pantilt_init(void) – intialise PWM function to drive the panTilt device
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "math.h"
#include "s4408736_hal_pantilt.h"

/* Private define ------------------------------------------------------------*/
#define PWM_CLOCKFREQ	50000
#define PWM_FREQUENCY	(int)(1/0.02)
#define PWM_PERIOD		2*PWM_CLOCKFREQ/PWM_FREQUENCY
#define PWM_CHANNEL		TIM_CHANNEL_1
#define PULSE_WIDTH1    (uint32_t)(PWM_PERIOD*7.25/100) //7.25% duty cycle = 0
#define PULSE_WIDTH2	(uint32_t)(PWM_PERIOD*2.25/100) //2.25% duty cycle = -90
#define PULSE_WIDTH3	(uint32_t)(PWM_PERIOD*12.25/100)//12.25% duty cycle +90
#define UPPER			85
#define LOWER			-85

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef TIM_Init;

/**
  * @brief  Initialise PWM function
  * @param  None
  * @retval None
  */
extern void s4408736_hal_pantilt_init (void) {

	//Initialize PWM pin
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure2;
	TIM_OC_InitTypeDef PWMConfig;

	uint16_t PrescalerValue = 0;

	__TIM1_CLK_ENABLE();
	__BRD_D6_GPIO_CLK();

	//Initialize pin D6
	GPIO_InitStructure.Pin = BRD_D6_PIN; //#define PWM_PIN
	GPIO_InitStructure.Mode =GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM1;//PWM_GPIO_AF;
	HAL_GPIO_Init(BRD_D6_GPIO_PORT, &GPIO_InitStructure);

	//Initialize pin D5
	GPIO_InitStructure2.Pin = BRD_D5_PIN; //#define PWM_PIN
	GPIO_InitStructure2.Mode =GPIO_MODE_AF_PP;
	GPIO_InitStructure2.Pull = GPIO_NOPULL;
	GPIO_InitStructure2.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure2.Alternate = GPIO_AF1_TIM1;//PWM_GPIO_AF;
	HAL_GPIO_Init(BRD_D5_GPIO_PORT, &GPIO_InitStructure2);

	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / PWM_CLOCKFREQ) - 1;

	//Set timer
	TIM_Init.Instance = TIM1;
	TIM_Init.Init.Period = PWM_PERIOD;
	TIM_Init.Init.Prescaler = PrescalerValue;
	TIM_Init.Init.ClockDivision = 0;
	TIM_Init.Init.RepetitionCounter = 0;
	TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;
	//Set PMW
	PWMConfig.OCMode = TIM_OCMODE_PWM1;
	PWMConfig.Pulse = PULSE_WIDTH1;
	PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
	PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
	PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	//Initialize CHANNEL1
	HAL_TIM_PWM_Init(&TIM_Init);
	HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_Init, TIM_CHANNEL_1);

	//Initialize CHANNEL2
	HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_Init, TIM_CHANNEL_2);
}

/**
  * @brief  Sets new position for device
  * @param  Type to choose where to write to
  * 		angle is the new angle for the device
  * @retval None
  */
void pantilt_angle_write(int type, int angle){

	double newAnglePulse = 0;
	//pan between -85 and +85 to prevent the motor to break down
	if (angle >= UPPER) {
		angle = UPPER;
	} else if (angle <= LOWER) {
		angle = LOWER;
	}

	//Pan and tilt
	//Set new pulse for PMW that depends on the angle
	if(type == 1){

		//tilt angle
		newAnglePulse = round( 145 + (angle * 100.0/90));
		__HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_2, newAnglePulse);
	} else if (type == 0) {

		//Pan angle
		newAnglePulse = round( 145 + (angle * 100.0/90));
		__HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_1, newAnglePulse);
	}
}

/**
  * @brief  Read current position of device
  * @param  type to know where to read from
  * @retval integer = current angle of device
  */
int pantilt_angle_read(int type){

	int anglePulse = 0;
	int angle = 0;

	//calculate angle according to pulse
	if(type == 1) {

		//Tilt
		anglePulse = __HAL_TIM_GET_COMPARE(&TIM_Init, TIM_CHANNEL_2);
		angle = round((anglePulse - 145) * 90.0/100.0);
	} else if (type == 0) {

		//Pan
		anglePulse = __HAL_TIM_GET_COMPARE(&TIM_Init, TIM_CHANNEL_1);
		angle = round((anglePulse - 145) * 90.0/100.0);
	}

	return angle;
}
