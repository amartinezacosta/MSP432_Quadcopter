#ifndef LED_H_
#define LED_H_

#include "ti_drivers_config.h"

#define GREEN_LED    CONFIG_GPIO_0
#define RED_LED      CONFIG_GPIO_1

#define Hz_35   28000
#define Hz_30   33000
#define Hz_20   50000
#define Hz_15   66000
#define Hz_10   100000
#define Hz_4    250000
#define Hz_2    500000

void LED_sequence(uint32_t led, uint32_t delay, uint32_t count);
void LED_sequence_both(uint32_t delay, uint32_t count);
inline void LED_solid(uint32_t led, uint32_t value);

#endif /* LED_H_ */
