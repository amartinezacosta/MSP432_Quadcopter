#ifndef PERIPHERAL_PWM_DEV_C_
#define PERIPHERAL_PWM_DEV_C_

#include <stdint.h>
#include <stdbool.h>

#define PWM_COUNT   4

#define PWM0        0
#define PWM1        1
#define PWM2        2
#define PWM3        3

void pwm_dev_init(uint32_t index);
void pwm_dev_period(uint32_t index, uint32_t period);
void pwm_dev_duty(uint32_t index, uint32_t duty);

#endif



