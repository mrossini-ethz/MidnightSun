#include "led_ctrl.h"
#include <util/delay.h>

#ifdef __AVR_ATmega328P__
# define OC1A_PIN   B1
# define ADC0_PIN   C0
#elif defined __AVR_ATmega32U4__
# define OC1A_PIN   B5
# define ADC0_PIN   F0
#endif

/* Lookup table that determines the TOP value for PWM for an input < DIM_PWM_MODE_THR */
int16_t intensity_lookup_freq [] = {65535, 49449, 37311, 28152, 21242, 16027, 12093, 9125, 6885};
/* Lookup table that determines the on-time for PWM for an input >= DIM_PWM_MODE_THR */
int16_t intensity_lookup_duty [] = {8, 11, 14, 19, 25, 33, 43, 57, 76, 101, 134, 177, 235, 311, 413, 547, 725, 961, 1273, 1687, 2236, 2964, 3928, 5206, 6900};
/* Top value that defines the highest frequency used for PWM */
#define DIM_PWM_TOP     6885
/* Minimum on-time for PWM */
#define DIM_PWM_MIN     8
/* Threshold of input value at which a changeover occurs between frequency controlled and on-time controlled */
#define DIM_PWM_MODE_THR    1024
/* Input range */
#define DIM_INPUT_MAX       4096

#define RX_BUFFER_LENGTH 64
char _rx_buffer [2][RX_BUFFER_LENGTH];
uint8_t _rx_buffer_pointer = 0;
uint8_t _rx_buffer_sel = 0;
uint8_t _rx_complete = 0;

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



/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */
static FILE USBSerialStream;


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	GlobalInterruptEnable();


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

    //int16_t data;
    //uint8_t msg;
    int16_t intensity;
    char msg[64];

    // serial echo loop
	for (;;)
	{
        cdc_receive();
        if (_rx_complete){
            intensity = atoi(_rx_buffer[_rx_buffer_sel]);
            sprintf(msg,"Set intensity to %d\n",intensity);
            set_intensity_pwm(intensity);
            CDC_Device_SendString(&VirtualSerial_CDC_Interface, msg);
            memset((void *) _rx_buffer[_rx_buffer_sel], '\0', RX_BUFFER_LENGTH);
            _rx_complete = 0;
        }
		//data = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
        //if (data>0){
        //msg = (uint8_t)data;
        //set_intensity_pwm(data*5000);
        //intensity = (data-48)*500;
        //set_intensity_pwm(intensity);
        //CDC_Device_SendByte(&VirtualSerial_CDC_Interface, msg);
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
        //} 
    }
    return 0;
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	USB_Init();
}

void cdc_receive(void){
     for (uint8_t i=0; i < CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);i++){
            if (_rx_buffer_pointer >= RX_BUFFER_LENGTH) {
                /* Reset at the beginning of the buffer after clearing the buffer */
                memset((void *) _rx_buffer[1 - _rx_buffer_sel], '\0', RX_BUFFER_LENGTH);
                _rx_buffer_pointer = 0;
            }
            char byte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
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
}


/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

