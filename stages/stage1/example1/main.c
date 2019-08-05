/**
 ******************************************************************************
 * @file    tm1_debounce.c
 * @author  Your Name - Your Student Number
 * @date    Today's Date
 * @brief   Debounces a mechanical button.
 ******************************************************************************
 *
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int currentTime = 0;
int lastTime = 0;
int LEDToggle = 0;
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);
void EXTI15_10_IRQHandler(void);

/**
  * @brief  Main program - flashes onboard LEDs, prints statement on button press
  * @param  None
  * @retval None
  */
void main(void)  {

	BRD_init();			/* Initalise board */
	Hardware_init();	/* Initalise hardware modules */

	/* Main processing loop */
    while (1) {

    	/* Check if your LED toggle variable, set from the interrupt, is indicating to toggle */
    	if(LEDToggle == 1){/* LED set to be toggled*/

    		/* Toggle an LED */
    		BRD_LEDRedToggle();
    		/* Print "LED Toggled!" with the value of your toggle variable */
    		debug_printf("LED Toggled %d\n\r", LEDToggle);
    		/* Reset your LED toggle variable */
    		LEDToggle = 0;

    	}

	}
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {

	/* Initialise LEDs */
	BRD_LEDInit();
	/* Turn off LEDs */
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();
	GPIO_InitTypeDef GPIO_InitStructure;

	BRD_USER_BUTTON_GPIO_CLK_ENABLE();
	//GPIO_MODE_IT_RISING OR GPIO_MODE_INPUT
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Pin  = BRD_USER_BUTTON_PIN;
	HAL_GPIO_Init( BRD_USER_BUTTON_GPIO_PORT , &GPIO_InitStructure);

	HAL_NVIC_SetPriority( BRD_USER_BUTTON_EXTI_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ( BRD_USER_BUTTON_EXTI_IRQn);
}

/**
 * @brief EXTI line detection callback
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	/* Check if interrupt triggered on expected pin */
	if (GPIO_Pin == GPIO_PIN_13/* Button Pin */)
	{
		/* Obtain current time, see HAL_GetTick() */
		currentTime = HAL_GetTick();
		/* Check if the interrupt has occurred outside the debounce threshold */
		/* current time is outside of the debounce threshold since last true button press */
			if( currentTime > (lastTime + 300)){

			/* Set the status of your LED toggle variable */
			LEDToggle = 1;
			/* Make sure the your true button press time matches the current time */
			lastTime = currentTime ;//+ 300;
		}
		//lastTime = currentTime;
	}
}


//Override default mapping of this handler to Default_Handler
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);/* Button Pin */
}
