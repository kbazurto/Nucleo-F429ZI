/********************************************************************************
 * @file    mylib/s4408736_hal_hamming.h
 * @author  Kevin Bazurto – 44087368
 * @date    11/04/2018
 * @brief   Hamming library
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_hamming_encoder(uint8_t in) – Encode 4 bits
 * s4408736_hal_hamming_decoder(uint8_t in) - Decode 8 bits
 ******************************************************************************
 */

#ifndef S4408736_HAL_HAMMING_H_
#define S4408736_HAL_HAMMING_H_

/* Private define ------------------------------------------------------------*/
//Variables used to check for errors in decoding
int hammingErrorCount;
uint8_t errorMask;

//define functions to access internal functions
extern uint8_t s4408736_hal_hamming_encoder(uint8_t in);
extern uint8_t s4408736_hal_hamming_decoder(uint8_t in);
extern uint8_t s4408736_decode(uint8_t firstByte, uint8_t secondByte);

#endif /* S4408736_HAL_HAMMING_H_ */
