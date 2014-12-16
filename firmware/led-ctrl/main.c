#include <avr/io.h>
#include <util/delay.h>

#include "lookup.h"

/* Lookup table that converts a linear range to an exponential function from 1 up to 8192 in 15 steps */
int16_t intensity_lookup [] = {1, 2, 3, 5, 9, 16, 28, 49, 84, 147, 256, 446, 776, 1351, 2353, 4096};

int main (void) {
	/* Set clock to 4 MHz by setting the clock division (prescale) to 2 */
	//clock_set_prescale_2();

	/* Delay to allow slave devices to start up */
	_delay_ms(10);
	// set pin PB0 to output (1)
	//DDRB |= (1<<PB0y);
	// set pin PB0 to input (0)
	//DDRB &= ~(1<<PB0);
	// set all GPIO A to input and pull-up
	DDRA = 0x00;
	DDRB = 0xf0;
	PORTA = 0xfe;
	PORTB = 0x0f;
	// set all GPIO to Output
	DDRC = 0xff;
	DDRD = 0xff;


	// Initialise 16-bit timer
	// Turn off Power Reduction Mode
	PRR &= ~(1<<PRTIM1);
	// Set OC1A pin as output
	DDRD |= (1<<PD5);
	// Set OC1A output compare pin to clear on upcount and set at downcount
	TCCR1A |= (1<<COM1A1);
	TCCR1A &= ~(1<<COM1A0);
	// Set mode to phase correct PWM with ICR as TOP
	TCCR1B |= (1<<WGM13);
	TCCR1B &= ~(1<<WGM12);
	TCCR1A |= (1<<WGM11);
	TCCR1A &= ~(1<<WGM10);
	// Define TOP value (frequency = f_io / (2 * N * TOP), N = prescale factor)
	ICR1 = 4096;
	// Set the duty cycle initially to 0/4096
	OCR1A = 0;
	// Set no clock prescale and start
	TCCR1B |= (1<<CS10);

	// setup ADC
	ADMUX = 0;
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);

	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);

	ADCSRB = 0;

	/* Main loop */
	while(1) {
		/* Start an ADC conversion */
		ADCSRA |= (1<<ADSC);

		/* Wait for the conversion to finish */
		while(ADCSRA & (1<<ADSC));

		/* Calculate intensity from ADC reading */
		uint16_t intensity;
		if (ADC < 16)
			intensity = 0;
		else
			intensity = lookup(ADC, intensity_lookup, 32, 992, 6);

		/* Set PWM duty cycle */
		OCR1A = intensity;

		/* Sleep */
		_delay_ms(10);
	} /* Main loop */

	/* The program will never reach this point. */
	return 0;
}
