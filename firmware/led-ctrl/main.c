#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#include "macros.h"
#include "lookup.h"
#include "interrupt.h"
#include "usart.h"

#ifdef __AVR_ATmega328P__
# define OC1A_PIN	B1
# define ADC0_PIN	C0
#elif defined __AVR_ATmega32U4__
# define OC1A_PIN	B5
# define ADC0_PIN	F0
#endif

/* Lookup table that determines the TOP value for PWM for an input < DIM_PWM_MODE_THR */
int16_t intensity_lookup_freq [] = {65535, 49449, 37311, 28152, 21242, 16027, 12093, 9125, 6885};
/* Lookup table that determines the on-time for PWM for an input >= DIM_PWM_MODE_THR */
int16_t intensity_lookup_duty [] = {8, 11, 14, 19, 25, 33, 43, 57, 76, 101, 134, 177, 235, 311, 413, 547, 725, 961, 1273, 1687, 2236, 2964, 3928, 5206, 6900};
/* Top value that defines the highest frequency used for PWM */
#define DIM_PWM_TOP		6885
/* Minimum on-time for PWM */
#define DIM_PWM_MIN		8
/* Threshold of input value at which a changeover occurs between frequency controlled and on-time controlled */
#define DIM_PWM_MODE_THR	1024
/* Input range */
#define DIM_INPUT_MAX		4096

void set_intensity_pwm(int16_t input) {
	if (input == 0) {
		/* Turn off output */
		OCR1A = 0;
		ICR1 = DIM_PWM_TOP;
		TCNT1 = 0;
		return;
	}

	if (input < DIM_PWM_MODE_THR) {
		/* Constant t-on, variable frequency */
		OCR1A = DIM_PWM_MIN;
		ICR1 = lookup(input, intensity_lookup_freq, 0, DIM_PWM_MODE_THR, 7);
		TCNT1 = 0;
	} else {
		/* Constant frequency, variable t-on */
		OCR1A = lookup(input, intensity_lookup_duty, DIM_PWM_MODE_THR, DIM_INPUT_MAX, 7);
		ICR1 = DIM_PWM_TOP;
		TCNT1 = 0;
	}
}

int main (void) {
	/* -- Set all GPIO pins to output --- */
	DDRB = 0xff;
	DDRC = 0xff;
	DDRD = 0xff;
#ifdef __AVR_ATmega32U4__
	DDRE = 0xff;
	DDRF = 0xff;
	DDRD = 0xff;
#endif

	/* --- Initialise 16-bit timer --- */
	/* Set OC1A pin as output */
	GPIO_SET_OUTPUT(OC1A_PIN);
	/* Set OC1A output compare pin to clear on upcount and set at downcount */
	SET_FLAGS(TCCR1A, COM1A1);
	/* Set mode to phase correct PWM with ICR as TOP */
	SET_FLAGS(TCCR1B, WGM13);
	SET_FLAGS(TCCR1A, WGM11);
	/* Define TOP value (frequency = f_io / (2 * N * TOP), N = prescale factor) */
	ICR1 = DIM_PWM_TOP;
	/* Set the duty cycle initially to 0/8192 */
	OCR1A = DIM_PWM_TOP;
	/* Set no clock prescale and start */
	SET_FLAGS(TCCR1B, CS10);

	/* --- Setup ADC --- */
	/* Set AREF to AVCC (5 V), use channel 0 */
	SET_FLAGS(ADMUX, REFS0);
	/* Set the pin belonging to channel 0 to input */
	GPIO_SET_INPUT(ADC0_PIN);
	/* Set clock prescale to 128 and enable */
	SET_FLAGS(ADCSRA, ADEN, ADPS0, ADPS1, ADPS2);

	/* --- Setup UART --- */
	USART_init();

	/* --- Turn on interrupts --- */
	interrupts_on();

	/* Main loop */
	while(1) {
		if (_rx_complete) {
			USART_send_bytes((uint8_t *) _rx_buffer[_rx_buffer_sel], strlen((char *) _rx_buffer[_rx_buffer_sel]));
			USART_send_bytes((uint8_t *) "\n", 1);
			_rx_complete = 0;
		}

		/* Start an ADC conversion */
		SET_FLAGS(ADCSRA, ADSC);

		/* Wait for the conversion to finish */
		while(GET_FLAG(ADCSRA, ADSC));

		/* Calculate intensity from ADC reading */
		uint16_t intensity = ADC * 4;
		set_intensity_pwm(intensity);

		/* Sleep */
		_delay_ms(10);
	} /* Main loop */

	/* The program will never reach this point. */
	return 0;
}
