/********************************************************************************
 * @file    mylib/s4408736_os_APCRadio.h
 * @author  Kevin Bazurto – 44087368
 * @date    20/05/2018
 * @brief   Task to control radio
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * extern void s4408736_os_apcradio_init(void) - initialize task and semaphores
 * extern void pen_up_down (int direction) - set Z axis coordinates (not send)
 ******************************************************************************
 */

#ifndef S4408736_OS_APCRADIO_H_
#define S4408736_OS_APCRADIO_H_

/* Private define ------------------------------------------------------------*/

//Used Semaphores for radio
SemaphoreHandle_t s4408736_SemaphoreRadioGetSys;
SemaphoreHandle_t s4408736_SemaphoreRadioGetChan;
SemaphoreHandle_t s4408736_SemaphoreRadioGetRxAddr;
SemaphoreHandle_t s4408736_SemaphoreRadioGetTxAddr;
SemaphoreHandle_t s4408736_SemaphoreRadioJoin;
SemaphoreHandle_t s4408736_SemaphoreRadioPenUp;
SemaphoreHandle_t s4408736_SemaphoreRadioPenDown;
SemaphoreHandle_t s4408736_SemaphoreRadioOrigin;

//Used Queues for radio
QueueHandle_t s4408736_QueueRadioChannel;
QueueHandle_t s4408736_QueueRadioRxAddr;
QueueHandle_t s4408736_QueueRadioTxAddr;
QueueHandle_t s4408736_QueueRadioPosition;
QueueHandle_t s4408736_QueueRadioSetHeight;
QueueHandle_t s4408736_QueueRadioGraphics;

//define functions to access internal functions
extern void s4408736_os_apcradio_init(void);
extern void pen_up_down (int direction);

#endif /* S4408736_OS_APCRADIO_H_ */

