#include "ESC.h"
#include "EasyHal/pwm_dev.h"
#include "EasyHal/time_dev.h"

#include <unistd.h>

//RANGE: 12,000 (1ms) - 24,000 (2ms) for SMCLK @ 12MHz!
void ESC_init(void)
{
    pwm_dev_init(PWM0);
    pwm_dev_init(PWM1);
    pwm_dev_init(PWM2);
    pwm_dev_init(PWM3);
}

//RANGE: 12,000 (1ms) - 24,000 (2ms) for SMCLK @ 12MHz!
void ESC_speed(uint32_t esc, uint32_t duty)
{
    if(duty > MAX_PWM_COUNT) duty = MAX_PWM_COUNT;
    if(duty < MIN_PWM_COUNT) duty = MIN_PWM_COUNT;

    pwm_dev_duty(esc, duty);
}

//RANGE: 12,000 (1ms) - 24,000 (2ms) for SMCLK @ 12MHz!
void ESC_arm(void)
{
    //Arm all 4 motors
    sleep(1);

    //Go minimum speed- 5% duty cycle - count 12000
    ESC_speed(ESC0, MIN_PWM_COUNT);
    ESC_speed(ESC1, MIN_PWM_COUNT);
    ESC_speed(ESC2, MIN_PWM_COUNT);
    ESC_speed(ESC3, MIN_PWM_COUNT);
    sleep(1);

    //Go full speed - 10% duty cycle - count 24000
    ESC_speed(ESC0, MAX_PWM_COUNT);
    ESC_speed(ESC1, MAX_PWM_COUNT);
    ESC_speed(ESC2, MAX_PWM_COUNT);
    ESC_speed(ESC3, MAX_PWM_COUNT);
    sleep(1);

    //Go minimum speed - 5% duty cycle - count 12000
    ESC_speed(ESC0, MIN_PWM_COUNT);
    ESC_speed(ESC1, MIN_PWM_COUNT);
    ESC_speed(ESC2, MIN_PWM_COUNT);
    ESC_speed(ESC3, MIN_PWM_COUNT);
    sleep(1);
}

//RANGE: 12,000 (1ms) - 24,000 (2ms) for SMCLK @ 12MHz!
void ESC_calibrate(void)
{
    sleep(1);

    //Full throttle
    ESC_speed(ESC0, MAX_PWM_COUNT);
    ESC_speed(ESC1, MAX_PWM_COUNT);
    ESC_speed(ESC2, MAX_PWM_COUNT);
    ESC_speed(ESC3, MAX_PWM_COUNT);
    sleep(4);

    //Minimum throttle
    ESC_speed(ESC0, MIN_PWM_COUNT);
    ESC_speed(ESC1, MIN_PWM_COUNT);
    ESC_speed(ESC2, MIN_PWM_COUNT);
    ESC_speed(ESC3, MIN_PWM_COUNT);
    sleep(4);
}





