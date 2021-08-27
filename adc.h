#include <stdint.h>

#define ADC_PIN         (1U << 3)
#define EMPTY_BIT       (1U << 8)

void adc_init(void);
uint8_t adc_fifo_empty(void);
uint16_t adc_fifo_read(void);
void led_init(void);