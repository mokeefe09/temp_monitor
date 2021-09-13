/*
Analog to Digital Converter driver. Written for a specific project. Not intended for general use.
*/

#include "adc.h"

void adc_init(void){
/*
Initializes the ADC module. The ADC will begin sampling when the processor indicates it should
by setting a bit in the PSSI register. In the greater scope of the project, this will allow the
software to only sample from the ADC when it needs to, instead of continuously sampling.
*/
	// Enable clock to ADC module 0
	SYSCTL->RCGCADC	= (1U << 0); 
  
	// Enable clock to GPIO port E
	SYSCTL->RCGCGPIO |= (1U << 4);
  
	// Set pin E3 to use alternate functions, disable its digital settings, and
	// enable analog function
	GPIOE->AFSEL  	|= ADC_PIN;
	GPIOE->DEN    	&= ~ADC_PIN;
	GPIOE->AMSEL  	|= ADC_PIN;
	  
	// Disable sample sequencer 3 while configuring
	ADC0->ACTSS   	&= (1U << 3);

	// Processor initiates sampling
	ADC0->EMUX    	= 0x0;

	// Sample from AIN1 (GPIOE pin 3)
	ADC0->SSMUX3  	= 0x0;

	// Enable sample sequencer 3
	ADC0->ACTSS   	= (1U << 3);

	// Disable sampling. The processor will set this bit when it wants to begin sampling
	ADC0->PSSI	&=	~(1U << 3);
}


uint16_t adc_sample(void){
/*
Samples the ADC and returns the 12-bit result stored in the FIFO. 
*/
	// Will be used to hold "garbage" reads from the sample sequencer FIFO
	uint16_t garbage_val;	

	// Make sure sampling is halted before accessing the FIFO.
	ADC0->PSSI	&=	~(1U << 3);

	// If result FIFO is full, read the register until it is empty. This makes sure
	// we get a fresh result from this sample and not an old result.
	while (ADC0->SSFSTAT3 & (1U << 12)){
		garbage_val = ADC0->SSFIFO3;	
	}
	
	// Now that the FIFO is empty, we can initiate sampling. Sample until the FIFO
	// has something in it. Then disable sampling.
	ADC0->PSSI	|= 	(1U << 3);
	while (ADC0->SSFSTAT3 & (1U << 8)) {};
	ADC0->PSSI	&=	~(1U << 3);

	// Return the value in the FIFO.
	return ADC0->SSFIFO3;
}
