#include <stdint.h>
#include "io_map.h"

#define ADC_PIN         PIN3
#define ADC_PORT	GPIOE

void adc_init(void);
uint16_t adc_sample(void);
