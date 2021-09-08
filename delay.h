#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>

// Shared global variable. Defined in main.c. Incremented every 1ms by a SysTick interrupt.
extern volatile uint32_t ms_value;

// Function prototype
void ms_delay(uint32_t d);

#endif
