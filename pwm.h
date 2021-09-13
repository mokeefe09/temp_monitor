#ifndef __PWM_H__
#define __PWM_H__

#include "TM4C123GH6PM.h"
#include "io_map.h"

#define PWM_PIN		PIN4

void pwm_init(void);
void pwm_enable(void);
void pwm_disable(void);

#endif
