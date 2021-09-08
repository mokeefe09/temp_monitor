#include <stdint.h>
#include "io_map.h"

#define ADC_PIN         PIN3
#define EMPTY_BIT       (1U << 8)
#define ADC_PORT	GPIOE

void adc_init(void);
uint8_t adc_fifo_empty(void);
uint16_t adc_fifo_read(void);
void led_init(void);
