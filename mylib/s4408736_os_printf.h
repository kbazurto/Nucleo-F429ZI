/********************************************************************************
 * @file    mylib/s4408736_os_printf.h
 * @author  Kevin Bazurto – 44087368
 * @date    18/05/2018
 * @brief   myprintf - prints from a task using a QUEUE
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *extern void s4408736_os_printf_init(void)
 *extern void s44087368_printf (const char *fmt, ...)
 ******************************************************************************
 */

#ifndef S4408736_OS_PRINTF_H_
#define S4408736_OS_PRINTF_H_

#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdarg.h>
#include "s4408736_os_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Private define ------------------------------------------------------------*/
#define myprintf	s44087368_printf

QueueHandle_t s4408736_QueuePrintf;	/* Queue used */

//define functions to access internal functions
extern void s4408736_os_printf_init(void);
extern void s44087368_printf(const char *fmt, ...);

#endif /* S4408736_OS_PRINTF_H_ */

