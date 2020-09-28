#include "pwm_dev.h"
#include "config_dev.h"

/*--------------------LOW LEVEL PWM HARDWARE IMPLEMENTATION, DEVICE SPECIFIC--------------------*/
/* TIMER_A0
 * P2.4 -> CCR1
 * P2.5 -> CCR2
 * P2.6 -> CCR3
 * P2.7 -> CCR4
 */
#ifdef MSP432P401R_RTOS_PWM

/*WARNING! DO NOT COMPILE THIS ON A DRA OR DRIVERLIB PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include "ti_drivers_config.h"
#include <ti/drivers/PWM.h>

PWM_Handle pwm_handles[PWM_COUNT];
bool  pwm_open[PWM_COUNT] = {false, false, false, false};
uint32_t pwm_configs[PWM_COUNT] = {CONFIG_PWM_0, CONFIG_PWM_1, CONFIG_PWM_2, CONFIG_PWM_3};

void pwm_dev_init(uint32_t index)
{
    if(pwm_open[index]) return;

    /*WARNING! Values based from SMCLK frequency
     * RTOS -> 12MHz
     * Driverlib or DRA -> 3MHz
     */

    //Default frequency = 1KHz
    //Default duty = 0%
    PWM_Params params;
    PWM_Params_init(&params);
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_COUNTS;
    params.periodValue = 12000;

    pwm_handles[index] = PWM_open(pwm_configs[index], &params);
    if(pwm_handles[index] == NULL)
    {
        //Failed to initalize pwm;
        while(1);
    }

    PWM_start(pwm_handles[index]);

    pwm_open[index] = true;
}

void pwm_dev_period(uint32_t index, uint32_t period)
{
    PWM_setPeriod(pwm_handles[index], period);
}

void pwm_dev_duty(uint32_t index, uint32_t duty)
{
    PWM_setDuty(pwm_handles[index], duty);
}

#elif defined(MSP432P401R_DRIVERLIB_PWM)

/*WARNING! DO NOT COMPILE THIS ON A DRA OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

bool  pwm_open[PWM_COUNT] = {false, false, false, false};
uint32_t system_pwm_clock;
uint32_t pins[PWM_COUNT] = {GPIO_PIN4, GPIO_PIN5, GPIO_PIN6, GPIO_PIN7};
uint32_t ccr[PWM_COUNT] = {TIMER_A_CAPTURECOMPARE_REGISTER_1, TIMER_A_CAPTURECOMPARE_REGISTER_2, TIMER_A_CAPTURECOMPARE_REGISTER_3, TIMER_A_CAPTURECOMPARE_REGISTER_4};
Timer_A_PWMConfig pwm_configs[PWM_COUNT] =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        0,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

void pwm_dev_init(uint32_t index)
{
    if(pwm_open[index]) return;

    /*WARNING! Values based from SMCLK frequency
     * RTOS -> 12MHz
     * Driverlib or DRA -> 3MHz
     */

    //Default frequency = 1KHz
    //Default duty = 0%

    pwm_configs[index].timerPeriod = (SMCLK_DEV_FREQUENCY/1000) - 1;
    pwm_configs[index].dutyCycle = 0;
    pwm_configs[index].compareRegister = ccr[index];

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, pins[index], GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwm_configs[index]);

    system_pwm_clock = system_clock;

    pwm_open[index] = true;
}

void pwm_dev_period(uint32_t index, uint32_t period)
{
    pwm_configs[index].timerPeriod = period;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwm_configs[index]);
}

void pwm_dev_period(uint32_t index, uint32_t duty)
{
    pwm_configs[index].dutyCycle = duty;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwm_configs[index]);
}

#elif defined(MSP432P401R_DRA_PWM)

/*WARNING! DO NOT COMPILE THIS ON A DRIVERLIB OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include "msp.h"

extern uint32_t SystemCoreClock;

bool pwm_open[PWM_COUNT] = {false, false, false, false};
uint32_t pwm_period_count;
const uint32_t pins[PWM_COUNT] = {BIT4, BIT5, BIT6, BIT7};

//TODO: This should not need the system_clock variable
void pwm_dev_init(uint32_t index)
{
    if(pwm_open[index]) return;

    //TODO: Make port mapping generic
    uint32_t pin = pins[index];
    uint32_t ccr = index + 1;

    //Set pin as output, select PWM functionality
    P2->DIR |= pin;
    P2->SEL0 |= pin;
    P2->SEL1 &= ~pin;

    /*WARNING! Values based from SMCLK frequency
     * RTOS -> 12MHz
     * Driverlib or DRA -> 3MHz
     */

    //Default frequency = 1KHz
    //Default duty = 0%
    //SMCLK clock, Count up to CCR0, clear timer counter
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
    TIMER_A0->CCTL[ccr] = TIMER_A_CCTLN_OUTMOD_7;

    //1KHz default frequency
    pwm_period_count = (SMCLK_DEV_FREQUENCY/1000) - 1;

    TIMER_A0->CCR[0] = pwm_period_count;
    TIMER_A0->CCR[ccr] = 0;

    pwm_open[index] = true;
}

void pwm_dev_period(uint32_t index, uint32_t period)
{
    pwm_period_count = period;
    TIMER_A0->CCR[0] = pwm_period_count - 1;
}

void pwm_dev_duty(uint32_t index, uint32_t duty)
{
    uint32_t ccr = index + 1;

    if(duty > pwm_period_count)
    {
        duty = pwm_period_count;
    }

    TIMER_A0->CCR[ccr] = duty - 1;
}


#else
#warning USING UN-IMPLEMENTED PWM, YOU MUST PROVIDE YOUR OWN SPECIFIC PWM INTERFACE

void pwm_dev_init(uint32_t index)
{

}

void pwm_dev_period(uint32_t index, uint32_t period)
{

}

void pwm_dev_duty(uint32_t index, uint32_t duty)
{

}


#endif

