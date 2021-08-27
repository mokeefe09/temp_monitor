#include "pwm.h"
#include "TM4C123GH6PM.h"

void pwm_init(void){
	// Enable clock to PWM 0 module
	SYSCTL->RCGCPWM = (1U << 0);
	  
	// Enable clock to GPIOB port
	SYSCTL->RCGCGPIO |= (1U << 1);
	  
	// Enable alternate functions for GPIOB pin 6
	GPIOB->AFSEL |= (1U << 6);
	  
	// Choose the specific alternate function for GPIOB (PWM)
	GPIOB->PCTL |= (0x4 << 24);
	  
	// Enable digigtal functions for GPIOB pin 6
	GPIOB->DEN |= (1U << 6);
	  
	// Use a pre-scaler (clock divisor) of 32 for the PWM clock. The system
	// clock is 16MHz. Divided by 32, the frequeny of the PWM clock becomes
	// 500KHz (500,000 cycles per second). Set bits 20 and 19, and clear bits
	// 18 and 17.
	SYSCTL->RCC |= (1U << 20) | (1U << 19);
	SYSCTL->RCC &= ~((1U << 18) | (1U << 17));
	  
	// Set the PWM 0 module to count-down mode, and disable it while config
	// changes are made
	PWM0->_0_CTL = 0;
	  
	// When comparator A is hit, drive PWM0A low. When 0 is hit on the counter,
	// drive PWM0A high.
	PWM0->_0_GENA = 0x83;
	  
	// Counter will start counting down from 124, to give a period of 125 cycles.
	// Using a 500KHz clock, this results in 4,000 cycles per second, which is
	// what is needed to sound the piezo buzzer.
	PWM0->_0_LOAD = 0x7C;
	  
	// Comparator set to 63, for just about a 50% duty cycle on the PWM. When
	// the comparator is hit by the counter, the pwm pin will be driven low.
	PWM0->_0_CMPA = 0x3F;
	  
	// Start the PWM timer
	PWM0->_0_CTL |= (1U <<0);
	  
	// Disable the PWM signal. Will be enabled in the main function.
	PWM0->ENABLE &= ~(1U << 0);
}
