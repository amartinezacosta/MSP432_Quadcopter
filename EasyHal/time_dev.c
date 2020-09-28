#include "time_dev.h"
#include "config_dev.h"

//TODO: Implement micros? delay_microseconds?

#ifdef MSP432P401R_RTOS_TIME

/*WARNING! DO NOT COMPILE THIS ON A DRA OR DRIVERLIB PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <ti/drivers/Timer.h>
#include <unistd.h>

#include "ti_drivers_config.h"

Timer_Handle timer;
volatile uint32_t timer_millis = 0;

bool timer_open = false;
void timer_dev_callback(Timer_Handle handle);

void time_dev_init(void)
{
    if(timer_open) return;

    Timer_Params params;
    Timer_Params_init(&params);
    params.period = 1000;       //1ms delay
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timer_dev_callback;

    timer = Timer_open(CONFIG_TIMER_0, &params);
    if(timer == NULL)
    {
        //Failed to open timer
        while(1);
    }

    if(Timer_start(timer) == Timer_STATUS_ERROR)
    {
        //Failed to start timer
        while(1);
    }

    timer_open = true;
}

void delay(uint32_t ms)
{
    usleep(1000*ms);
}

uint32_t millis(void)
{
    return timer_millis;
}

void timer_dev_callback(Timer_Handle handle)
{
    timer_millis++;
}

#elif defined(MSP432P401R_DRIVERLIB_TIME)

/*WARNING! DO NOT COMPILE THIS ON A DRA OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <EasyHal/config_dev.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

volatile uint32_t timer_millis = 0;

void time_dev_init(void)
{
    Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_BASE, (SMCLK_DEV_FREQUENCY/1000) - 1);

    /* Enabling interrupts */
    Interrupt_enableInterrupt(INT_T32_INT1);
    Interrupt_enableMaster();

    Timer32_startTimer(TIMER32_BASE, true);
}

void delay(uint32_t ms)
{
    uint32_t start = millis();

    while((millis()-start) < ms)
    {
        //yield();
    }
}

uint32_t millis(void)
{
    return timer_millis;
}

void T32_INT1_IRQHandler(void)
{
    Timer32_clearInterruptFlag(TIMER32_BASE);
    timer_millis++;
}

#elif defined(MSP432P401R_DRA_TIME)

/*WARNING! DO NOT COMPILE THIS ON A DRIVERLIB OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include "msp.h"

bool timer_open = false;
volatile uint32_t timer_millis = 0;

void time_dev_init(void)
{
    if(timer_open) return;
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_MODE;
    TIMER32_1->LOAD = (SMCLK_DEV_FREQUENCY/1000) - 1;

    NVIC_EnableIRQ(T32_INT1_IRQn);

    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE;

    timer_open = true;
}

void delay(uint32_t ms)
{
    uint32_t start = millis();

    while((millis()-start) < ms)
    {
        //yield();
    }
}

uint32_t millis(void)
{
    return timer_millis;
}

void T32_INT1_IRQHandler(void)
{
    TIMER32_1->INTCLR |= BIT0;

    timer_millis++;
}

#else
#warning USING UN-IMPLEMENTED TIME, YOU MUST PROVIDE YOUR OWN SPECIFIC TIMER INTERFACE

void time_dev_init(void)
{

}

void delay(uint32_t ms)
{

}

uint32_t millis(void)
{
    return 0;
}

//Timer ISR implementation
void TIMER_ISR(void)
{

}

#endif





