#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "adc.h"

void adc_init(void){
	// Enable clock to ADC module 0
	SYSCTL->RCGCADC = (1U << 0); 
  
	// Enable clock to GPIO port E
	SYSCTL->RCGCGPIO |= (1U << 4);
  
	// Set pin E3 to use alternate functions, disable its digital settings, and
	// enable analog function
	ADC_PORT->AFSEL  |= ADC_PIN;
	ADC_PORT->DEN    &= ~ADC_PIN;
	ADC_PORT->AMSEL  |= ADC_PIN;
	  
	// Disable sample sequencer 3 while configuring
	ADC0->ACTSS   &= (1U << 3);

	// Continuously sample
	ADC0->EMUX    = (0xF << 12);

	// Sample from AIN1 (GPIOE pin 3)
	ADC0->SSMUX3  = 0x0;

	// Enable sample sequencer 3
	ADC0->ACTSS   = (1U << 3);

	// Initiate sampling
	ADC0->PSSI    = (1U << 3);
}

uint8_t adc_fifo_empty(void){
	// If FIFO is empty return true
	if (ADC0->SSFSTAT3 & EMPTY_BIT)
	{
		return 1;
	}
	// If FIFO not empty, return false
	else{
		return 0;
	}
}

uint16_t adc_fifo_read(void){
	// Return value in ADC FIFO.
	return ADC0->SSFIFO3;
}
