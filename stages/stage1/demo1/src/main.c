/**
  ******************************************************************************
  * @file    demo1/main.c
  * @author  Kevin Bazurto 44087368
  * @date    01/03/2018
  * @brief   Nucleo429ZI onboard LED flashing example.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_ledbar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned short LEDPosition = 0x300;
int delayVariable = 1000;
int increaseDecrease = 1;
int currentTime = 0;
int lastTime = 0;
int changeTime = 0;
/* Private function prototypes -----------------------------------------------*/
void EXTI15_10_IRQHandler(void);
void Hardware_init(void);
/**
  * @brief  Main program - controls LED board
  * @param  None
  * @retval None
  */
void main(void)  {
	//Initalise Board and pins
	BRD_init();
	Hardware_init();
	s4408736_hal_ledbar_init();
	int direction = 1;
    //main loop
	while (1) {
		//handle LEDS
    	s4408736_hal_ledbar_write(LEDPosition);
    	if(changeTime == 1){
    		/* change period*/
    	    if (delayVariable >= 1000) {
    	    	increaseDecrease = 1;
    	    } else if (delayVariable < 100) {
    	    	increaseDecrease = 0;
    	    }
    	    if (increaseDecrease == 1) {
    	    	delayVariable = delayVariable/2;
    	    	debug_printf("mayor a 1000: %d\n\r ", delayVariable);
    	    } else {
    	    	delayVariable = delayVariable*2;
    	    	debug_printf("menor a 100: %d\n\r ", delayVariable);
    	    }
    	    changeTime = 0;
    	}
    	//handle direction
    	if (LEDPosition == 0x003 ) {
    		direction = 0;
    	} else if (LEDPosition == 0x300) {
    		direction = 1;
    	}
    	if (direction == 1 ) {
    		LEDPosition = LEDPosition >> 1;
    	} else {
    		LEDPosition = LEDPosition << 1;
    	}
    	debug_printf("MPD: 0x%x\n\r ", LEDPosition);
		HAL_Delay(delayVariable);
    }
}

/**
 * Initialize push button as an external interrupt
 */
void Hardware_init(void) {
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
 * Handles external interrupt
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	/* Check if interrupt triggered on expected pin */
	if (GPIO_Pin == GPIO_PIN_13/* Button Pin */) {
		/* Obtain current time, see HAL_GetTick() */
		currentTime = HAL_GetTick();
		/* Check if the interrupt has occurred outside the debounce threshold */
		if( currentTime > (lastTime + 300)){
			changeTime = 1;
			lastTime = currentTime;
		}
	}
}

//Override default mapping of this handler to Default_Handler
void EXTI15_10_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
