/**
	\file
	\ingroup Interrupts
	\brief Definitions of functions and variables related to interrupts

	\addtogroup Interrupts
	\{
 */

#include <string.h>
#include "macros.h"
#include "interrupt.h"
#include "usart.h"
#include "lookup.h"

uint8_t sreg_save;

extern uint16_t dim_curve [];
extern uint8_t dim_curve_points;
extern volatile uint16_t _dim_curve_position;
extern uint8_t dim_curve_resolution;

extern volatile uint16_t _intensity;

extern void set_intensity_pwm(int16_t input);

/**
	\brief USART0 RX complete interrupt handler

	This interrupt handler is called whenever a new byte has been received
	by the USART interface.
	It reads the incoming byte and appends it to a the string \ref _rx_buffer
	until a newline character is encountered.
	If that happens the \ref _rx_complete flag is set.
	It stores a maximum of \ref RX_BUFFER_LENGTH characters.
	If the buffer is full, it is cleared and the incoming character is written
	into the emptied buffer.

	See also \ref USART "here".
 */
ISR(USART_RX_vect) {
	/* Make sure the buffer does not overflow */
	if (_rx_buffer_pointer >= RX_BUFFER_LENGTH) {
		/* Reset at the beginning of the buffer after clearing the buffer */
		memset((void *) _rx_buffer[1 - _rx_buffer_sel], '\0', RX_BUFFER_LENGTH);
		_rx_buffer_pointer = 0;
	}

	char byte = UDR0;

	/* Check whether the current character marks the end of the message */
	if (byte != '\n') {
		/* Read the next character into the buffer */
		_rx_buffer[1 - _rx_buffer_sel][_rx_buffer_pointer] = byte;

		/* Increment the buffer pointer */
		_rx_buffer_pointer++;
	} else {
		/* Set RX complete flag that is handled in the main routine */
		_rx_complete = 1;

		/* Switch buffers */
		_rx_buffer_sel = 1 - _rx_buffer_sel;

		/* Reset the buffer pointer */
		_rx_buffer_pointer = 0;
	}
}

ISR(TIMER0_OVF_vect) {
	set_intensity_pwm(lookup(_dim_curve_position, (int16_t *) dim_curve, 0, ((uint16_t) (dim_curve_points - 1)) * (1 << dim_curve_resolution), dim_curve_resolution));

	/* Increment position */
	if (_dim_curve_position < ((uint16_t) (dim_curve_points - 1)) * (1 << dim_curve_resolution)) {
		_dim_curve_position++;
	} else {
		/* Wrap around */
		_dim_curve_position = 0;
		/* Turn off timer */
		TCCR0B = 0;
	}
}

/** \} */
