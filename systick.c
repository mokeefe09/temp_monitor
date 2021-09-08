#include "systick.h"

void SysTick_Handler(void){
	ms_value++;
}

void SysTick_init(void){
	// Disable systick and interrupts while making changes
	SysTick->CTRL	&=	~((1U << 0) | (1U << 1));

	// With a 16 MHz clock, a period of 16,000 cycles will 
	// equal a period of 1ms.
	SysTick->LOAD	=	15999U;

	// Writing any value to this register clears it. This
	// reg holds the counter value for the SysTick timer. When
	// it hits 0 it rolls over and generates an interrupt.
	SysTick->VAL	=	0;

	// Enable interrupts for the SysTick module, enable the module
	// itself, and use the system clock for the timer.
	SysTick->CTRL	|=	(1U << 2) | (1U << 1) | (1U << 0);

	// Enable SysTick interrupt in NVIC
	NVIC->ISER[0] 	= 	(1U << 15);
}

