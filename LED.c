#include "LED.h"

#include <ti/drivers/GPIO.h>

#include <unistd.h>

void LED_sequence(uint32_t led, uint32_t delay, uint32_t count)
{
    uint32_t i;
    for(i = 0; i < count; i++)
    {
        GPIO_write(led, 0);
        usleep(delay);
        GPIO_write(led, 1);
        usleep(delay);
    }
}

void LED_sequence_both(uint32_t delay, uint32_t count)
{
    uint32_t i;
    for(i = 0; i < count; i++)
    {
        GPIO_write(RED_LED, 0);
        GPIO_write(GREEN_LED, 0);
        usleep(delay);
        GPIO_write(RED_LED, 1);
        GPIO_write(GREEN_LED, 1);
        usleep(delay);
    }
}

inline void LED_solid(uint32_t led, uint32_t value)
{
    GPIO_write(led, value);
}
