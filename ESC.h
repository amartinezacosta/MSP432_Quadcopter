#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>

#define ESC0    PWM0
#define ESC1    PWM1
#define ESC2    PWM2
#define ESC3    PWM3

#define MIN_PWM_COUNT       12000
#define MAX_PWM_COUNT       24000

void ESC_init(void);
void ESC_speed(uint32_t esc, uint32_t duty);
void ESC_arm(void);
void ESC_calibrate(void);

#endif /* SERVO_H_ */
