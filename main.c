/*
Author: Michael O'Keefe
Date: 09/13/2021
Description: Device that digitally displays temperature on an LCD. When a programmable maximum
	     temperature is exceeded, a piezo buzzer alarm is triggered and a warning message
	     is displayed on the screen. 
*/


#include "adc.h"
#include "pwm.h"
#include "lcd.h"
#include "systick.h"
#include "itoa_temp.h"
#include "TM4C123GH6PM.h"
#include <stdint.h>
#include <math.h>

#define RES_1		(float)10000			// Resistance of 1st volt divider resistor
#define ADC_MAX_VAL	(float)4095			// Resolution of ADC
#define ROOM_T		(float)298.15			// "Room temperature" in Kelvin
#define ROOM_R		(float)10000			// Resistance of thermistor at room temp
#define B_VALUE		(float)3950			// B-value of thermistor
#define CELS_CONV       ( (float)9 / (float)5 )		// Celsius conversion factor
#define MAX_TEMP	78				// The max temp before buzzer and warning

#define STR_LEN		30				// Length of buffer used to store string
							// that displays temperature.

#define DL_INTERVAL	2000U				// Delay in between temp check tasks (in ms)

// This value is incremented every 1ms in a SysTick timer. It is used to determine if the deadline
// interval between tasks has been reached, as well as in delay functions. Since the value is
// modified in an ISR, it should be declared volatile.
volatile uint32_t ms_value = 0;

int main(void){
	uint32_t	deadline = 100U;		// Task will run for first time when ms_value
							// reaches 100.

	uint16_t 	adc_value;			// 12-bit voltage reading from ADC.

	float 		thermistor;			// Resistance of thermistor (changes when temp
							// changes

	float 		kelvin_temp;			// Temperature placeholders
	float 		cels_temp;
	float 		fahr_temp;
	int8_t 		integer_temp;

	uint8_t		max_flag = 0;			// If this flag is 1, then the last time through
							// the task we were above the max temp. If 0, we
							// were at or below the max temp. The flag is
							// needed to know if we should clear screen before
							// displaying the next string.

	char 		display_string[STR_LEN] = {0};		// Buffer to hold temperature string

	
	char*		current_string = "Current Temp:";	// Other display strings
	char* 		degrees_string = "DEG";
	char*		warning_string1 = "WARNING: Max";
	char*		warning_string2 = "temp exceeded";

	// Initialize peripherals
	SysTick_init();
	lcd_init();
	adc_init();
	pwm_init();

	while (1){
		// Has the appropriate # of ms passed before we run the task again?
		if (ms_value >= deadline){

			// Set new deadline for task
			deadline += DL_INTERVAL;

			// Raw 12-bit voltage value from ADC stored in adc_value
			adc_value = adc_sample();

			// Calculate the value of the thermistor (knowing the thermistor
			// resistance is the first step to determining the temperature).
			// RES_1 is the resistance of the fixed resistor in the voltage divider.
			// ADC_MAX_VAL is the max value the ADC can contain.
			//
			// CITATION: The formula and the reasoning behind it can be found
			// in the adafruit article titled "Using a Thermistor". 
			// url: https://learn.adafruit.com/thermistor/using-a-thermistor

			thermistor = (RES_1 / ((ADC_MAX_VAL / (float)adc_value) - (float)1 ));

			// Use the 'simplified B parameter equation' to calculate the temperature
			// from the resistance of the second thermometer. ROOM_T is room temp in
			// kelvin, thermistor is the resistance of the thermistor, ROOM_R is the
			// resistance at the room temperature, ROOM_T. B_VALUE is the b value or
			// 'beta value' of the resistor. 
			//
			// CITATION: The formula and the reasoning behind it can be found
			// in the adafruit article titled "Using a Thermistor". 
			// url: https://learn.adafruit.com/thermistor/using-a-thermistor

			kelvin_temp = (float)1 / (( (float)1 / ROOM_T ) + (logf(thermistor/ROOM_R) / B_VALUE));
			cels_temp = kelvin_temp - (float)273.15;
			fahr_temp = (cels_temp * CELS_CONV) + (float)32;
			integer_temp = (int8_t)fahr_temp;

                        // Is the integer representation of the temperature at or below the max?
			if (integer_temp <= MAX_TEMP) {

				// If we were above the max temp in the previous task, clear the display
				// to make sure there are no display errors from leftover data and clear
				// the flag
				if (max_flag == 1){
					lcd_send_cmd(DISP_CLR);
					max_flag = 0;
				}

				// Convert integer temperature to string version and store in display_string.
				// Will contain the string version of the integer followed by "degrees"
				itoa_temp(integer_temp, display_string, degrees_string, STR_LEN);

				// Write the temperature to the lcd display
				lcd_display(0, 0, current_string);
				lcd_display(0, 1, display_string);

				// Turn off buzzer
				pwm_disable();
			}
			else{

				// If we were at or below the max temp in the previous task, clear the display
				// to make sure there are no display errors from leftover data and set the
				// flag.
				if (max_flag == 0){
					lcd_send_cmd(DISP_CLR);
					max_flag = 1;
				}

				// print warning strings
				lcd_display(0, 0, warning_string1);
				lcd_display(0, 1, warning_string2);

				// Sound buzzer
				pwm_enable();
			}

		} // if (ms_value >= deadline)

	} // while(1)

} // main
