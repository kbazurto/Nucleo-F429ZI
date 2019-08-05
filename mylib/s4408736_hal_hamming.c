/**
 ******************************************************************************
 * @file    mylib/s4408736_hal_hamming.c
 * @author  Kevin Bazurto – 44087368
 * @date    11/03/2018
 * @brief   hamming encoding/decoding library
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4408736_hal_hamming_encoder(void) – Encode data using hamming
 * s4408736_hal_hamming_decoder(void) – Decode data using hamming
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_hamming.h"
#include "s4408736_os_printf.h"

/**
  * @brief  Reset variable that check for errors
  * @param  None
  * @retval None
  */
void reset_decoding_variables(void){

	hammingErrorCount = 0;
	errorMask = 0;
}

/**
  * @brief  Decode 4 bits into Hamming
  * 		Implement Hamming Code + parity checking
  * 		Hamming decode is based on the following matrix
  * 	 	H =[ 1 0 0 | 1 1 1 0 ;
  *   			 0 1 0 | 1 0 1 1 ;
  *   			 0 0 1 | 1 1 0 1 ];
  * @param  in - Data to be decoded
  * @retval 8 bit decoded data.
  */
extern uint8_t s4408736_hal_hamming_decoder(uint8_t in) {

	uint8_t copy = in;
	uint8_t d0, d1, d2, d3, d4, d5, d6, d7;
	uint8_t p0 = 0;
	uint8_t s0, s1, s2;
	uint8_t syndrome;

	//extract bits
	d0 = !!(in & 0x1);
	d1 = !!(in & 0x2);
	d2 = !!(in & 0x4);
	d3 = !!(in & 0x8);
	d4 = !!(in & 0x10);
	d5 = !!(in & 0x20);
	d6 = !!(in & 0x40);
	d7 = !!(in & 0x80);

	//generate syndrome
	s0 = d1^d4^d5^d6;
	s1 = d2^d4^d6^d7;
	s2 = d3^d4^d5^d7;
	syndrome = (s0 | (s1 << 1) | (s2 << 2));

	/* calculate even parity bit */
	for (int z = 1; z < 8; z++){
		p0 = p0 ^ !!(in & (1 << z));
	}

	//check where is the error
	switch(syndrome){

	case 0:

		if (p0 != d0) {

			//error on parity bit
			copy ^= 1;
			d0 ^= 1;
			hammingErrorCount++;
		}
		break;

	case 1:

		//error on bit 1
		copy ^= (1 << 1);
		d1 ^= 1;
		hammingErrorCount++;
		break;

	case 2:

		//error on bit 2
		copy ^= (1 << 2);
		hammingErrorCount++;
		d2 ^= 1;
		break;

	case 3:

		//error on bit 6
		copy ^= (1 << 6);
		d6 ^= 1;
		hammingErrorCount++;
		break;

	case 4:

		//error on bit 3
		copy ^= (1 << 3);
		d3 ^= 1;
		hammingErrorCount++;
		break;

	case 5:

		//error on bit 5
		copy ^= (1 << 5);
		hammingErrorCount++;
		d5 ^= 1;
		break;

	case 6:

		//error on bit 7
		copy ^= (1 << 7);
		hammingErrorCount++;
		d7 ^= 1;
		break;

	case 7:

		//error on bit 4
		copy ^= (1 << 4);
		hammingErrorCount++;
		d4 ^= 1;
		break;
	}

	//generate mask
	errorMask = copy ^ in;

	//Check for 2 errors
	p0 = 0;

	for (int z = 1; z < 8; z++){
		p0 = p0 ^ !!(copy & (1 << z));
	}

	if (p0 != d0) {
		hammingErrorCount++;
	}

	return (d4 | (d5 << 1) | (d6 << 2) | (d7 << 3));
}

/**
  * @brief  Encode 4 bits into Hamming
  * 		Implement Hamming Code + parity checking
  * 		Hamming code is based on the following generator
  * 		G = [ 1 1 1 | 1 0 0 0 ;
  *       		  1 0 1 | 0 1 0 0 ;
  *       		  1 1 0 | 0 0 1 0 ;
  *       		  0 1 1 | 0 0 0 1 ;
  * @param  in - Data to be encoded
  * @retval 8 bit encoded data.
  */
extern uint8_t s4408736_hal_hamming_encoder(uint8_t in) {

	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits */
	d0 = !!(in & 0x1);
	d1 = !!(in & 0x2);
	d2 = !!(in & 0x4);
	d3 = !!(in & 0x8);

	/* calculate hamming parity bits */
	h0 = d0 ^ d1 ^ d2;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z<8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;
	return(out);
}

/**
  * @brief  Decode 2 byte word
  * @param  First byte is MSByte, secnd byte is LSByte
  * @retval 8 bit decoded data.
  */
extern uint8_t s4408736_decode(uint8_t firstByte, uint8_t secondByte) {

	int error1, error2;
	int16_t mask = 0;
	int8_t decoded = 0;

	//decode message byte by byte
	reset_decoding_variables();
	decoded = s4408736_hal_hamming_decoder(firstByte) << 4;
	error1 = hammingErrorCount;
	mask = (errorMask << 8);

	//Second byte
	reset_decoding_variables();
	decoded |= s4408736_hal_hamming_decoder(secondByte);
	error2 = hammingErrorCount;
	mask |= errorMask;

	reset_decoding_variables();

	//Decide if it was a 2 bit error or not
	if (error2 == 2 || error1 == 2) {
		myprintf("\n\r2-bit ERROR\n\r");
	}

	return decoded;
}
