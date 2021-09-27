#include "delay.h"

void ms_delay(uint32_t d){
/*
Delay function delays the number of milliseconds specified by the input
parameter, d. Relies on a SysTick timer that updates a global variable,
ms_value, once every ms.
*/
	// Set start and now variables to the current value of the ms counter
	uint32_t start = ms_value, now = ms_value;

	// We need the "now" variable to be (d + 1) greater than  the "start"
	// variable to avoid a fencepost error. 
	while ((now - start) < (d + 1)){
		now = ms_value;
	}

}

void nano_delay(void){
}

void tiny_delay(uint32_t d){
	while (--d != 0){}
}
