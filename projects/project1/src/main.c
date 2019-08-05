/**
  ******************************************************************************
  * @file    project1/main.c
  * @author  Kevin Bazurto 44087368
  * @date    04/03/2018
  * @brief   3010 Project1
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "math.h"
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4408736_hal_pantilt.h"
#include "s4408736_hal_joystick.h"
#include "s4408736_hal_ir.h"
#include "s4408736_hal_hamming.h"
#include "s4408736_hal_ircomms.h"
#include "s4408736_hal_ledbar.h"

/* Private typedef -----------------------------------------------------------*/
TIM_HandleTypeDef TIM4_Init;

/* Private define ------------------------------------------------------------*/
#define IGNORE 0
#define PASS 1
#define IDLE 1
#define TERMINAL_CONTROL 2
#define JOYSTICK_CONTROL 3
#define ENCODE_DECODE 4
#define IR_DUPLEX 5
#define DECODE 6
#define ENCODE 4
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define CHARACTER 0
#define STRING 1
#define WAIT_ACK 3000
#define WAIT_NEXT_CHARACTER 120
#define WAIT_STOP_BIT 11
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//Variables used to process modes
char input;
char encodingInput;
char buffer[6];
int status;
int statusIgnore = PASS;
int transmmitNow = IGNORE;

//Varaibles for mode 2-3
int panAngle = 0;
int tiltAngle = 0;

//Variables used for IR transmit
char transmissionBuffer[14];
int position = 0;
int transmissionPosition = 0;
int counterIr = 0;
int startBit = 0;
int endBit = 0;
uint16_t encodingWord;
uint16_t trasmittingWord;
unsigned int input_capture_value = 0;

//Variables used for IR received
int currentTime = 0;
int oldTime = 0;
uint32_t received = 0;
volatile int bitCount = 0;
int time;

//IR duplex
uint16_t userString[11]= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char transmissionString[13];
int characterCounter = 0;
int characterString = CHARACTER;
int ackCounter = 0;
int ackTime = 0;
int ackCurrentTime = 0;
int ackOldTime = 0;
uint8_t decodedReceived;
char receivedString[11];
int receivedCounter = 0;
int receivedFlag = CHARACTER;
char acknowledgement[3] = {STX, ACK, ETX};

//Variables to print every second
int panTiltCurrentTime = 0;
int panTiltOldTime = 0;
int panTiltTime = 0;

//Variables for main loop indicator and LEDBAR
int ledTime = 0;
int ledOldTime = 0;
int ledCurrentTime = 0;
unsigned short LEDPosition = 0x000;
int modeID = 0;

/* Private function prototypes -----------------------------------------------*/
void TIM4_IRQHandler(void);

/**
  * @brief  Decode a 16 bit data using manchester encoding.
  * @param  encodedMessage - Data to be decoded
  * @retval 8 bit decoded data.
  */
uint8_t manchester_decode(uint16_t encodedMessage) {

	uint8_t decodedMessage;

	//Take bits in odd positions
	uint8_t first = !!(encodedMessage & 1);
	uint8_t second = !!(encodedMessage & 4);
	uint8_t third = !!(encodedMessage & 16);
	uint8_t fourth = !!(encodedMessage & 64);
	uint8_t fifth = !!(encodedMessage & 256);
	uint8_t sixth = !!(encodedMessage & 1024);
	uint8_t seventh = !!(encodedMessage & 4096);
	uint8_t eigth = !!(encodedMessage & 16384);

	//Join each bit in the correct number
	decodedMessage = (first | (second << 1) | (third << 2) | (fourth << 3)
			| (fifth << 4) | (sixth << 5) | (seventh << 6) | (eigth << 7));
	return decodedMessage;
}

/**
  * @brief  Encode an 8 bit data using manchester encoding.
  * @param  byte - Data to be encoded
  * @retval 16 bit enoded data.
  */
uint16_t manchester_encode(char byte) {

	uint16_t encodedByte;
	uint8_t encodingPart = ~byte;

	// Take each bit of the data
	uint8_t first = !!(byte & 1);
	uint8_t second = !!(encodingPart & 1);
	uint8_t third = !!(byte & 2);
	uint8_t fourth = !!(encodingPart & 2);
	uint8_t fifth = !!(byte & 4);
	uint8_t sixth = !!(encodingPart & 4);
	uint8_t seventh = !!(byte & 8);
	uint8_t eigth = !!(encodingPart & 8);

	//Join each bit in the correct number
	encodedByte = (first | (second << 1) | (third << 2) | (fourth << 3)
			| (fifth << 4)|(sixth << 5) | (seventh << 6) | (eigth << 7));
	return encodedByte;
}

/**
  * @brief  Converts a character into its equivalent value
  * 		ex: '4' to 0x4
  * @param  limit - limit to iterate over buffer
  * @retval None
  */
void char_to_int(int limit) {

	position =  0;
	statusIgnore = PASS;

	for(int j = 2; j < limit; j++) {

		//Number 0-9 //48 == 0 57 == 9
		if (buffer[j] >= 48 && buffer[j] <= 57) {
			buffer[j] = buffer[j] - 48;
		//Letters A-F
		} else if (buffer[j] >= 65 && buffer[j] <= 70) {
			buffer[j] = buffer[j] - 55;
		}
	}
}

/**
  * @brief  resets buffer used
  * @param  None
  * @retval None
  */
void clear_buffer(void){
	for(int i = 0; i < DECODE; i++){
		buffer[i] = '\0';
	}
}

/**
  * @brief	Resets variables for Hamming decoding
  * @param  None
  * @retval None
  */
void reset_decoding_variables2(void){
	hammingErrorCount = 0;
	errorMask = 0;
}

/**
  * @brief	Decides if the user wants to encode or decode
  * 		using manchester or hamming
  * @param  None
  * @retval None
  */
void encode_decode_handler(void){

	uint16_t encodingWord;
	uint8_t decodedWord;
	//Store input
	if (input != '\0') {

		buffer[position] = input;
		debug_printf("%c", buffer[position]);
		position++;
	}
	//Manchester
	if (buffer[0] == 'M') {

		//DECODE
		if(buffer[1] == 'D' && position != 0) {

			statusIgnore = IGNORE;
			if (position == DECODE ){

				char_to_int(DECODE);
				decodedWord = manchester_decode(buffer[5]|(buffer[4] << 4)
						|(buffer[3] << 8)|(buffer[2] << 12));
				debug_printf("\n\r%X\n\r", decodedWord);
				clear_buffer();
			}
			//ENCODE
		} else if (buffer[1] == 'E' && position != 0) {

			statusIgnore = IGNORE;
			if(position == ENCODE){

				char_to_int(ENCODE);
				encodingWord = ((manchester_encode(buffer[2]) << 8)
						| (manchester_encode(buffer[3])));
				debug_printf("\n\r%X\n\r", encodingWord);
				clear_buffer();
			}
		}
	//Hamming
	} else if (buffer[0] == 'H') {

		//DECODE
		if(buffer[1] == 'D' && position != 0){

			statusIgnore = IGNORE;
			if (position == DECODE){

				char_to_int(DECODE);
				//variables to store decode message, mask and full parameters
				int error1, error2;
				int16_t full = 0;
				int16_t mask = 0;
				int8_t decoded = 0;

				//decode message byte by byte
				reset_decoding_variables();
				decoded = s4408736_hal_hamming_decoder((buffer[2] << 4)
						| (buffer[3])) << 4;
				error1 = hammingErrorCount;
				mask = (errorMask << 8);

				//Second byte
				reset_decoding_variables();
				decoded |= s4408736_hal_hamming_decoder((buffer[4] << 4)
						| (buffer[5]));
				error2 = hammingErrorCount;
				mask |= errorMask;

				reset_decoding_variables();
				full = (buffer[2] << 12) | (buffer[3] << 8) | (buffer[4] << 4)
						| (buffer[5]);

				//Decide if it was a 2 bit error or not
				if (error2 == 2 || error1 == 2) {
					debug_printf("\n\r2-bit ERROR\n\r");
				} else {
					debug_printf("\n\r%X (Full:%04hhX ErrMask:%04hhX)\n\r",
							decoded,(mask ^ full),mask);
				}
				clear_buffer();
			}
		//ENCODE
		} else if (buffer[1] == 'E' && position != 0) {

			statusIgnore = IGNORE;
			if (position == ENCODE){

				char_to_int(ENCODE);
				uint16_t hamming = 0;
				hamming = ((s4408736_hal_hamming_encoder(buffer[2]) << 8)
						| (s4408736_hal_hamming_encoder(buffer[3])));
				debug_printf("\n\r%X\n\r", hamming);
				clear_buffer();
			}
		}
	}
}

/**
  * @brief	Decides if the user wants to encode or decode
  * 		using manchester or hamming
  * @param  None
  * @retval None
  */
void ir_receive(void){

	//variables to separate data
	char MSB;
	char LSB;
	//Data has been recevied
	if (bitCount >= 21) {

		LEDPosition &= ~(1 << 4);
		HAL_Delay(WAIT_STOP_BIT);
		decodedReceived = manchester_decode((received >> 2));

		//Check if data received is a string or character
		if (decodedReceived == STX || receivedFlag) {

			//store characters into buffer
			if (decodedReceived != STX && decodedReceived != ETX) {
				receivedString[receivedCounter] = decodedReceived;
				receivedCounter++;
			}
			receivedFlag = STRING;
		}

		//Sent ACK after receiving a string
		if (decodedReceived == ETX || receivedCounter == 11) {

			//print string character by character
			debug_printf("Received from IR: ");
			for(int k = 0; k < receivedCounter; k++){
				debug_printf("%c", receivedString[k]);
			}
			debug_printf("\n\r");

			//decode ACK and send it
			for(int k = 0; k < 3; k++){

				MSB = 0xf & (acknowledgement[k] >> 4);
				LSB = 0xf & (acknowledgement[k]);
				encodingWord = ((manchester_encode(MSB)<< 8)
						| manchester_encode(LSB));
				trasmittingWord = encodingWord;
				transmmitNow = PASS;
				HAL_Delay(WAIT_NEXT_CHARACTER);
			}

			debug_printf("Sent from IR: ACK\n\r");
			receivedCounter = 0;
			receivedFlag = CHARACTER;
		}

		//Print data received from IR
		if (decodedReceived == ACK) {
			debug_printf("Received from IR: ACK\n\r", decodedReceived);
		} else if (receivedFlag == CHARACTER && decodedReceived != ETX){
			debug_printf("Received from IR: %c\n\r", decodedReceived);
		}

		bitCount = 0;
		received = 0;
	}
}

/**
  * @brief  Resets input buffer for IR mode
  * @param  None
  * @retval None
  */
void reset_ir_buffer(void){

	for (int k = 0; k < transmissionPosition; k++) {
		transmissionBuffer[k] = '\0';
	}
	transmissionPosition = 0;
}

/**
  * @brief  Decides if the user wants to Receive or transmit
  * @param  None
  * @retval None
  */
void ir_handler(void){
	//variables to separate data and detect when user ends inputting data
	char MSB;
	char LSB;
	int finished = PASS;

	ir_receive();

	//When user press enter starts transmitting
	if (input == '\r' && statusIgnore == IGNORE) {
		debug_printf("\n\r");
		statusIgnore = PASS;
		//If string then add ETX and STX
		if (transmissionPosition > 3) {

			transmissionString[0] = STX;
			for (int j = 1; j < (transmissionPosition - 1); j++) {
				transmissionString[j] = transmissionBuffer[j + 1] ;
			}

			transmissionString[transmissionPosition-1] = ETX;
			characterString = STRING;
		}

		//transmit string 3 times if no ACK
		if (characterString) {

			while (ackCounter < 3 && decodedReceived != 0x06) {

				debug_printf("Sent from IR: ");

				//transmmit each character
				for(int k = 0; k <= (transmissionPosition - 1); k++){
					MSB = 0xf & (transmissionString[k] >> 4);
					LSB = 0xf & (transmissionString[k]);
					encodingWord = ((manchester_encode(MSB)<< 8)
							| manchester_encode(LSB));
					trasmittingWord = encodingWord;
					transmmitNow = PASS;
				  	LEDPosition |= (1 << 3);
				  	s4408736_hal_ledbar_write(LEDPosition);
					HAL_Delay(WAIT_NEXT_CHARACTER);
				}

				//Print each character
				for (int k = 1; k <= (transmissionPosition - 2); k++) {
					debug_printf("%c", transmissionString[k]);
				}

				debug_printf("\n\r");
				ackCounter++;
				HAL_Delay(WAIT_ACK);
				ir_receive();
			}

			LEDPosition &= ~(1 << 3);
			s4408736_hal_ledbar_write(LEDPosition);
			//Give up after 3 transmits
			if (ackCounter == 3){
				debug_printf("After 3 transmits giving up\n\r");
			}

			ackCounter = 0;
			decodedReceived = 0;

		//Transmit character
		} else {

			debug_printf("Sent from IR: %c\n\r", transmissionBuffer[2]);
				MSB = 0xf & (transmissionBuffer[2] >> 4);
				LSB = 0xf & (transmissionBuffer[2]);
				encodingWord = ((manchester_encode(MSB)<< 8)
						| manchester_encode(LSB));
				trasmittingWord = encodingWord;
				transmmitNow = PASS;
		}

		finished = IGNORE;
		statusIgnore = PASS;
		characterString = CHARACTER;

		//clear buffer
		reset_ir_buffer();
	}

	//fill buffer
	if(input != '\0' && finished && input != '\r' ){

		transmissionBuffer[transmissionPosition] = input;
		debug_printf("%c", transmissionBuffer[transmissionPosition]);
		transmissionPosition++;

		//make sure the command exists
		if (transmissionBuffer[0] == 'I' && transmissionBuffer[1] == 'T' ) {
			statusIgnore = IGNORE;
		}
		if (transmissionBuffer[0] != 'I' && transmissionBuffer[1] != 'T'
				&& transmissionPosition == 2) {
			reset_ir_buffer();
		}
	}
}

/**
  * @brief  Moves the pan/tilt device by using the joystick
  * @param  None
  * @retval None
  */
void joystick_handler(void){

	//reads joystick value and converst it to angle using equation
	panAngle = ((150 * s4408736_hal_joystick_x_read())/4095) - 75;
	tiltAngle = ((150 * s4408736_hal_joystick_y_read())/4095) - 75;
	panTiltCurrentTime = HAL_GetTick();
	panTiltTime += panTiltCurrentTime - panTiltOldTime;

	//writes new angle
	s4408736_hal_pantilt_pan_write(panAngle);
	s4408736_hal_pantilt_tilt_write(tiltAngle);

	//Print after1 second
	if (panTiltTime >= 1000) {

		debug_printf("Pan: %d Tilt: %d\n\r", s4408736_hal_pantilt_pan_read(),
					s4408736_hal_pantilt_tilt_read());
			panTiltTime = 0;
	}
	panTiltOldTime = panTiltCurrentTime;}

/**
  * @brief  Moves the pan/tilt device by using the terminal
  * @param  None
  * @retval None
  */
void terminal_handler(void){

	//read current angle
	panAngle = s4408736_hal_pantilt_pan_read();
	tiltAngle = s4408736_hal_pantilt_tilt_read();
	panTiltCurrentTime = HAL_GetTick();
	panTiltTime += panTiltCurrentTime - panTiltOldTime;

	//TILT
	if (input == 'w' || input == 'W') {
		s4408736_hal_pantilt_tilt_write(tiltAngle + 1);
	} else if (input == 's' || input == 'S') {
		s4408736_hal_pantilt_tilt_write(tiltAngle - 1);
	//PAN
	} else if (input == 'd' || input == 'D') {
		s4408736_hal_pantilt_pan_write(panAngle + 1);
	} else if (input == 'a' || input == 'A') {
		s4408736_hal_pantilt_pan_write(panAngle - 1);
	}

	//Print after1 second
	if (panTiltTime >= 1000) {

		debug_printf("Pan: %d Tilt: %d\n\r", s4408736_hal_pantilt_pan_read(),
					s4408736_hal_pantilt_tilt_read());
			panTiltTime = 0;
	}
	panTiltOldTime = panTiltCurrentTime;

}

/**
  * @brief  Decides the function of each mode
  * @param  None
  * @retval None
  */
void status_handler(void){

	switch(status){

	case TERMINAL_CONTROL:
		terminal_handler();
		break;
	case JOYSTICK_CONTROL:
		joystick_handler();
		break;
	case ENCODE_DECODE:
		encode_decode_handler();
		break;
	case IR_DUPLEX:
		ir_handler();
		break;
	}
}

/**
  * @brief  Sets LEDs according t status
  * @param  None
  * @retval None
  */
void show_status(void){

	//extract bits
	uint8_t d0 = !!(modeID & 1);
	uint8_t d1 = !!(modeID & 2);
	uint8_t d2 = !!(modeID & 4);

	//write to LEDBAR
	if(d0 == 1){
		LEDPosition |= (1 << 8);
	} else {
		LEDPosition &= ~(1 << 8);
	}

	if(d1 == 1){
		LEDPosition |= (1 << 7);
	} else {
		LEDPosition &= ~(1 << 7);
	}

	if(d2 == 1){
		LEDPosition |= (1 << 6);
	} else {
		LEDPosition &= ~(1 << 6);
	}

	s4408736_hal_ledbar_write(LEDPosition);
}

/**
  * @brief  Takes user input and decides what mode to go into
  * @param  None
  * @retval None
  */
void input_handler(void){

	// Receive characters using getc
	input = debug_getc();

	//Usar can input numbers
	if (statusIgnore) {

		//select status
		switch (input) {

		//prints mode and set status flag
		case '?':

			debug_printf("1 Idle\n\r2 P/T Terminal\n\r3 P/T Joystick\n\r4 "
					"Encode/Decode\n\r5 IR Duplex\n\r");
			reset_ir_buffer();
			break;
		case '1':

			debug_printf("Mode 1 Idle\n\r");
			status = IDLE;
			modeID = 0;
			reset_ir_buffer();
			break;
		case '2':

			debug_printf("Mode 2 P/T Terminal\n\r");
			status = TERMINAL_CONTROL;
			modeID = 1;
			s4408736_hal_pantilt_init();
			reset_ir_buffer();
			break;
		case '3':

			debug_printf("Mode 3 P/T Joystick\n\r");
			status = JOYSTICK_CONTROL;
			modeID = 2;
			s4408736_hal_pantilt_init();
			reset_ir_buffer();
			break;
		case '4':

			debug_printf("Mode 4 Encode/Decode\n\r");
			status = ENCODE_DECODE;
			position = 0;
			input = '\0';
			modeID = 3;
			reset_ir_buffer();
			break;
		case '5':

			debug_printf("Mode 5 IR Duplex\n\r");
			status = IR_DUPLEX;
			modeID = 4;
			s4408736_hal_ir_init();
			input = '\0';
			break;
		default:
			show_status();
		}
	}
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void){
	BRD_init();
	s4408736_hal_ledbar_init();
	s4408736_hal_joystick_init();
	s4408736_hal_ircomms_init();
}

/**
  * @brief  Main program - controls interaction between user and pantilt device
  * @param  None
  * @retval None
  */
void main(void)  {

	Hardware_init();
	setbuf(stdout, NULL);

	while (1) {

		//Handle main loop indicator
		ledCurrentTime = HAL_GetTick();
		ledTime += ledCurrentTime - ledOldTime;

		if (ledTime >= 100) {

			LEDPosition ^= (1 << 9);
			s4408736_hal_ledbar_write(LEDPosition);
			ledTime = 0;
		}
		ledOldTime = ledCurrentTime;

		input_handler();
		status_handler();
	}
}

/**
 * @brief Period elapsed callback in non blocking mode
 * 			Transmit
 * @param htim: Pointer to a TIM_HandleTypeDef that contains the configuration
 * 				 information for the TIM module.
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	//Ready to transmit
	if (htim->Instance == TIM3 && transmmitNow) {

		//send encoded start bit
		if (counterIr >= 0 && counterIr <= 3) {

			LEDPosition |= (1 << 5);
			s4408736_hal_ledbar_write(LEDPosition);
			if (startBit == 0) {
				s4408736_hal_ir_datamodulation_clr();
			} else {
				s4408736_hal_ir_datamodulation_set();
			}
			startBit = ~startBit;

		//send encoded stop bit
		} else if (counterIr >= 20) {

			endBit = ~endBit;
			if (endBit == 0) {

				//finished so reset variables
				s4408736_hal_ir_datamodulation_clr();
				counterIr = -1;
				LEDPosition &= ~(1 << 5);
				s4408736_hal_ledbar_write(LEDPosition);
				transmmitNow = IGNORE;
			} else {
				s4408736_hal_ir_datamodulation_set();
			}

		//transmit encoded data bits
		} else {

			if (!!(trasmittingWord & 32768) == 0) {
				s4408736_hal_ir_datamodulation_clr();
			} else {
				s4408736_hal_ir_datamodulation_set();
			}
			trasmittingWord = trasmittingWord << 1;
		}
		counterIr++;
	}
}

//Override default mapping of this handler to Default_Handler
void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&TIM_InitP);
}


/**
  * @brief  Timer 4 Input Capture Interrupt handler
  * 		IR receive
  * @param  None.
  * @retval None
  */
void HAL_TIM4_IRQHandler (TIM_HandleTypeDef *htim) {

	/* Clear Input Capture Flag */
	__HAL_TIM_CLEAR_IT(&TIM4_Init, TIM_IT_TRIGGER);
	/* Read and display the Input Capture value of Timer 4, channel 3 */
  	HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);

  	//use time to check if data received 2 consecutive 1's or 0's
  	currentTime = HAL_GetTick();
  	time = currentTime - oldTime;
  	LEDPosition |= (1 << 4);
  	s4408736_hal_ledbar_write(LEDPosition);
  	//read pin and add 0 or 1 depending on value
  	if (HAL_GPIO_ReadPin(BRD_D10_GPIO_PORT, BRD_D10_PIN) == 1){

  		//add 2 1's
  		if (time >= 9) {

  			received |= 1;
  			received = received << 1;
  			received |= 1;
  			received = received << 1;
  			bitCount += 2;
  		//add 1
  		} else {

  			received |= 1;
  			received = received << 1;
  			bitCount ++;
  		}
  	} else {

  		//add 2 0's
  		if (time >= 9) {

  			received = received << 2;
  			bitCount += 2;
  		//add 0
  		} else {

  			received = received << 1;
  			bitCount ++;
  		}
  	}
   	oldTime = currentTime; // update time
}

/**
  * @brief  Timer 4 Input Capture Interrupt handler
  * Override default mapping of this handler to Default_Handler
  * @param  None.
  * @retval None
  */
void TIM4_IRQHandler(void) {
	HAL_TIM4_IRQHandler(&TIM4_Init);
}
