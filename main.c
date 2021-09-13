#include "adc.h"
#include "pwm.h"
#include "lcd.h"
#include "systick.h"
#include "itoa_temp.h"
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
#define MAX_TEMP	78
#define STR_LEN		30
#define DL_INTERVAL	2000U

volatile uint32_t ms_value = 0;

int main(void){
	uint32_t	deadline = 100U;
	uint16_t 	adc_value;
	float 		thermistor;
	float 		kelvin_temp;
	float 		cels_temp;
	float 		fahr_temp;
	int8_t 		integer_temp;
	uint8_t		max_flag = 0;

	char 		display_string[STR_LEN] = {0};
	char*		current_string = "Current Temp:";
	char* 		degrees_string = "DEG";
	char*		warning_string1 = "WARNING: Max";
	char*		warning_string2 = "temp exceeded";

	SysTick_init();
	lcd_init();
	adc_init();
	pwm_init();

	while (1){
		if (ms_value >= deadline){
			// Set new deadline for task
			deadline += DL_INTERVAL;

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

			//lcd_send_cmd(DISP_CLR);
                        
			if (integer_temp <= MAX_TEMP) {
				if (max_flag == 1){
					lcd_send_cmd(DISP_CLR);
				}
				// Clear display before writing?

				// print temperature
				// Convert integer temperature to string version and store in display_string.
				// Will contain the string version of the integer followed by "degrees"
				itoa_temp(integer_temp, display_string, degrees_string, STR_LEN);
				lcd_display(0, 0, current_string);
				lcd_display(0, 1, display_string);

				// Turn off buzzer
				PWM0->ENABLE &= ~(1U << 4);
				max_flag = 0;
			}
			else{

				if (max_flag == 0){
					lcd_send_cmd(DISP_CLR);
				}
				// Clear display before writing?

				// print warning strings
				lcd_display(0, 0, warning_string1);
				lcd_display(0, 1, warning_string2);

				// Sound buzzer
				PWM0->ENABLE |= (1U << 4);
				max_flag = 1;
			}
			
		}
	}
}
