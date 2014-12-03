#include <avr/io.h>
#include <util/delay.h>

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


    // initialise 16-bit timer
    // turn off PRM
    PRR &= ~(1<<PRTIM1);
    // set as output
    DDRD |= (1<<PD5);
    // set output compare pin to non-inverting PWM
    TCCR1A |= (1<<COM1A1); 
    TCCR1A &= ~(1<<COM1A0); 
    // Fast PWM
    TCCR1A |= (1<<WGM11); 
    TCCR1A &= ~(1<<WGM10); 
    TCCR1B |= (1<<WGM12);
    TCCR1B |= (1<<WGM13);
    // define TOP value
    ICR1 = 8192;
    // define duty cycle
    OCR1A = 2048;
    // set no prescale and start
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
        //uint16_t setting = ~((uint16_t) PINA);
        //uint16_t setting = 0;
        //setting += (~PINA) & 0xff;
        //setting += ((uint16_t)((~PINB) & 0x0f))<<8;
        //if (setting) {
        //    //OCR1A = ((uint16_t) 8192) / setting;
        //    OCR1A = setting << 1;
        //} else {
        //    OCR1A = 8192;    
        //}
        //toggle bit PB0
        // PORTB ^= (1<<PB0);
        
        // read ADC
        
        ADCSRA |= (1<<ADSC);
        while(ADCSRA & (1<<ADSC));
        OCR1A = (1<<((uint16_t)(((uint32_t)ADC*(uint32_t)53710)>>22))) - 1;

	    _delay_ms(10);
    } /* Main loop */

	/* The program will never reach this point. */
	return 0;
}
