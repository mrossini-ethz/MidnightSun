#include <avr/io.h>
#include <util/delay.h>

#include "macros.h"
#include "lookup.h"

#ifdef __AVR_ATmega328P__
# define OC1A_PIN	B1
# define ADC0_PIN	C0
#elif defined __AVR_ATmega32U4__
# define OC1A_PIN	B5
# define ADC0_PIN	F0
#endif

/* Lookup table that converts a linear range to an exponential function from 1 up to 8192 in 15 steps */
int16_t intensity_lookup [] = {1, 2, 3, 6, 11, 20, 37, 67, 122, 223, 406, 741, 1351, 2464, 4493, 8192};

int main (void) {
	/* Set clock to 2 MHz by setting the clock division (prescale) to 8 */
	SET_FLAGS(CLKPR, CLKPCE);
	SET_FLAGS(CLKPR, CLKPS1, CLKPS0);

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
	ICR1 = 8192;
	/* Set the duty cycle initially to 0/8192 */
	OCR1A = 0;
	/* Set no clock prescale and start */
	SET_FLAGS(TCCR1B, CS10);

	/* --- Setup ADC --- */
	/* Set AREF to AVCC (5 V), use channel 0 */
	SET_FLAGS(ADMUX, REFS0);
	/* Set the pin belonging to channel 0 to input */
	GPIO_SET_INPUT(ADC0_PIN);
	/* Set clock prescale to 128 and enable */
	SET_FLAGS(ADCSRA, ADEN, ADPS0, ADPS1, ADPS2);

	/* Main loop */
	while(1) {
		/* Start an ADC conversion */
		SET_FLAGS(ADCSRA, ADSC);

		/* Wait for the conversion to finish */
		while(GET_FLAG(ADCSRA, ADSC));

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
