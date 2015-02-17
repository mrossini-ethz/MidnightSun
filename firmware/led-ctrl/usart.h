/**
	\file
	\ingroup USART
	\brief Declarations of functions, variables, and constants related to the serial interface

	\addtogroup USART Serial interface
	\brief Protocol for communication with a computer

	Serial communication is used between microcontroller and computer.
	The serial data is transferred through a USB to serial converter outside
	the microcontroller.

	The communication is entirely passive.
	Commands are issued by the computer, processed on the microcontroller, and then a
	response is sent.
	All commands send a response, even if it is only an acknowledgement.
	The commuication protocol is human readable ASCII in SCPI style and commands and
	responses are terminated with a newline character.

	The reception of characters from the computer is interrupt driven.
	The interrupt handler puts together a serial command and stores it
	in one of the two \ref _rx_buffer arrays.
	When a newline is received the \ref _rx_complete flag is set and the
	\ref _rx_buffer_sel is toggled.
	This means that the receiving buffer is double buffered.

	The sending of responses to the computer is single buffered and not
	interrupt driven.
	Characters in the transmit buffer are sent one after another, blocking after each
	character until transmission is complete.

	\{
 */

#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

/* *** USART constants ************************************************************************************************************************************* */

#define RX_BUFFER_LENGTH 64	///< Buffer size for received characters
#define TX_BUFFER_LENGTH 64	///< Buffer size for transmitting characters

/* *** USART variables ************************************************************************************************************************************* */

extern volatile char _rx_buffer [2][RX_BUFFER_LENGTH];	///< Buffers used for receiving serial commands
extern volatile uint8_t _rx_buffer_pointer;		///< Pointer within the \ref _rx_buffer for writing the next character
extern volatile uint8_t _rx_buffer_sel;			///< Pointer to the active receiving buffer
extern volatile uint8_t _rx_complete;			///< Flag that indicates complete reception of a serial command
extern char tx_buffer [TX_BUFFER_LENGTH];		///< Buffer used for sending serial responses

/* *** USART functions ************************************************************************************************************************************* */

void USART_init(void);						///< Initializes the USART interface
void USART_send_bytes(const uint8_t * data, uint8_t length);	///< Sends a number of bytes through the USART interface
void USART_receive_bytes(uint8_t * data, uint8_t length);	///< Receives a number of bytes from the USART interface

/** \} */

#endif /* __USART_H__ */
