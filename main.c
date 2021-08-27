#include "adc.h"
#include "pwm.h"
#include "TM4C123GH6PM.h"
#include <stdint.h>

#define THRESHOLD 9100

int main(void){
  uint16_t adc_result;
  float thermistor;
  uint16_t other_therm;
  
  adc_init();
  pwm_init();
  
  
  while(1){
    // Wait until fifo has a result
    while (adc_fifo_empty());
    
    // Read the ADC result
    adc_result = adc_fifo_read();
    
    // Calculate resistance of the thermistor.
    thermistor = 10000.0 / ((4095.0 / (float)adc_result) - 1.0);
    other_therm = (uint16_t) thermistor;
    
    if (other_therm < THRESHOLD){
      // Enable the PWM signal
      PWM0->ENABLE |= (1U << 0);

    }
    else{
      // Disable the PWM signal
      PWM0->ENABLE &= ~(1U << 0);
    }
    
    
    
    
  }
  
}

