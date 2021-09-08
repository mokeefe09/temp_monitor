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

	// Processor initiates sampling
	ADC0->EMUX    = 0x0;

	// Sample from AIN1 (GPIOE pin 3)
	ADC0->SSMUX3  = 0x0;

	// Enable sample sequencer 3
	ADC0->ACTSS   = (1U << 3);
}


uint16_t adc_sample(void){
	// Will be used to hold "garbage" reads form the sample sequencer FIFO
	uint16_t garbage_val;	

	// Since we halt sampling at the end of this function, this line is likely
	// superfluous. But we will halt sampling at the start of the function as
	// well just to be safe.
	ADC0->PSSI	&=	~(1U << 3);

	// If result FIFO is full, read the register until it is empty
	while (ADC0->SSFSTAT3 & (1U << 12)){
		garbage_val = ADC0->SSFIFO3;	
	}
	
	// Now that the FIFO is empty, we can initiate sampling. Sample until the FIFO
	// has something in it. Then disable sampling and return that value.
	ADC0->PSSI	|= 	(1U << 3);
	while (ADC0->SSFSTAT3 & (1U << 8)) {};
	ADC0->PSSI	&=	~(1U << 3);

	// Return the value from the sample sequencer, which will be the voltage 
	// provided by the voltage divider.
	return ADC0->SSFIFO3;
}
