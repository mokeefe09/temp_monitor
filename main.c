#include "adc.h"
#include "pwm.h"
#include "lcd.h"
#include "systick.h"
#include "TM4C123GH6PM.h"
#include <stdint.h>
#include <math.h>

#define THRESHOLD 	(float)9100
#define RES_1		(float)10000
#define ADC_MAX_VAL	(float)4095
#define ROOM_T		(float)298.15
#define ROOM_R		(float)10000
#define B_VALUE		(float)3950
#define CELS_CONV       ( (float)9 / (float)5 )
#define MAX_TEMP	79
#define STR_LEN		30

volatile uint32_t ms_value = 0;

int main(void){
	uint32_t	deadline = 100U;
	uint16_t 	adc_value;
	float 		thermistor;
	float 		kelvin_temp;
	float 		cels_temp;
	float 		fahr_temp;
	int8_t 		integer_temp;

	char 		display_string[STR_LEN] = {0};
	char*		current_string = "Current Temp:";
	char* 		degrees_string = "degrees";
	char*		warning_string1 = "WARNING: Max";
	char*		warning_string2 = "temp exceeded";
	
        
        // TODO: Remove
        int8_t new_temp;

	SysTick_init();
	lcd_init();
	adc_init();
	pwm_init();

	while (1){
		if (ms_value >= deadline){
			// Task will run once per second
			deadline += 1000U;

			// Raw voltage value from ADC stored in adc_value
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
                        
                        // TODO: Just for debugging. Remove everything between here and next comment
                        new_temp = integer_temp * 100;
                        if (new_temp)
                          deadline = 0;

			
			// TODO: This is the end of the debugging section. Everything below here is legit
			// project code.
			if (integer_temp > MAX_TEMP) {
				// Clear display before writing?
				// print temperature

				// Turn off buzzer
			}
			else{

				// Clear display before writing?
				// print warning strings
				// Sound buzzer
			}
			
		}
	}
}
