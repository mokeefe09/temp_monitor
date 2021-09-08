#include "delay.h"

void ms_delay(uint32_t d){
	uint32_t start = ms_value, now = ms_value;

	while ((now - start) < (d + 1)){
		now = ms_value;
	}

}
