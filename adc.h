#ifndef __ADC_H__
#define __ADC_H__

#include "TM4C123GH6PM.h"
#include <stdint.h>
#include "io_map.h"

#define ADC_PIN         PIN3

void adc_init(void);
uint16_t adc_sample(void);

#endif
