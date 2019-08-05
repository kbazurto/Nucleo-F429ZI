/********************************************************************************
 * @file    mylib/s4408736_hal_radio.h
 * @author  Kevin Bazurto – 44087368
 * @date    21/03/2018
 * @brief   Radio driver
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *void s4408736_hal_radio_init();
 *void s4408736_hal_radio_init(void);
 *void s4408736_hal_radio_fsmprocessing(void);
 *void s4408736_hal_radio_setchan(unsigned char channel);
 *void s4408736_hal_radio_settxaddress(unsigned char *address);
 *void s4408736_hal_radio_setrxaddress(unsigned char *address);
 *unsigned char s4408736_hal_radio_getchan(void);
 *void s4408736_hal_radio_gettxaddress(unsigned char *address);
 *void s4408736_hal_radio_getrxaddress(unsigned char *address);
 *void s4408736_hal_radio_sendpacket(char channel, unsigned char *address, unsigned char *txpacket);
 *void s4408736_hal_radio_set_fsmrx(void);
 *void s4408736_hal_radio_getpacket(unsigned char *rxpacket);
 ******************************************************************************
 */
#ifndef S4408736_HAL_RADIO_H_
#define S4408736_HAL_RADIO_H_
/* Private define ------------------------------------------------------------*/

//define functions to access internal functions
extern void s4408736_hal_radio_init(void);
extern void s4408736_hal_radio_fsmprocessing(void);
extern void s4408736_hal_radio_setchan(unsigned char channel);
extern void s4408736_hal_radio_settxaddress(unsigned char *address);
extern void s4408736_hal_radio_setrxaddress(unsigned char *address);
extern unsigned char s4408736_hal_radio_getchan(void);
extern void s4408736_hal_radio_gettxaddress(unsigned char *address);
extern void s4408736_hal_radio_getrxaddress(unsigned char *address);
extern void s4408736_hal_radio_sendpacket(char channel, unsigned char *address, unsigned char *txpacket);
extern void s4408736_hal_radio_set_fsmrx(void);
extern void s4408736_hal_radio_getpacket(unsigned char *rxpacket);
extern int s4408736_hal_radio_getrxstatus(void);

#endif /* S4408736_HAL_RADIO_H_ */
