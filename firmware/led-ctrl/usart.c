/**
	\file
	\ingroup USART
	\brief Definition of functions and variables related to the serial interface
 */

#include <avr/io.h>
#include "usart.h"
#include "macros.h"

/* FIXME: Allocate dynamically to save flash memory */
volatile char _rx_buffer [2][RX_BUFFER_LENGTH];
volatile uint8_t _rx_buffer_sel;
volatile uint8_t _rx_buffer_pointer;
volatile uint8_t _rx_complete;
char tx_buffer [TX_BUFFER_LENGTH];

void USART_init(void)
{
	/* Disable the power reduction for the USART */
	CLR_FLAGS(PRR, PRUSART0);

	/* Use default values: Asynchronous mode, 8 bit character size, no parity, 1 stop bit */

	/* Set the baud rate to 9600 */
	UBRR0 = 12;

	/* Enable the receiver and transmitter */
	SET_FLAGS(UCSR0B, RXEN0, TXEN0);

	/* Enable the interrupt when data arrives */
	SET_FLAGS(UCSR0B, RXCIE0);
}

void USART_send_bytes(const uint8_t * data, uint8_t length)
{
	uint8_t i;
	/* Loop through all bytes */
	for (i = 0; i < length; i++) {
		/* Wait until the transmit buffer is empty */
		while (!GET_FLAG(UCSR0A, UDRE0));

		/* Send one byte */
		UDR0 = data[i];
	}
}

void USART_receive_bytes(uint8_t * data, uint8_t length)
{
	uint8_t i;
	for (i = 0; i < length; i++) {
		/* Wait for data to be received */
		while (!GET_FLAG(UCSR0A, RXC0));

		/* Receive one byte */
		data[i] = UDR0;
	}
}
