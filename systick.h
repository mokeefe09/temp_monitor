#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>
#include "TM4C123GH6PM.h"

extern volatile uint32_t ms_value;

void SysTick_init(void);

#endif
