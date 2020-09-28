#ifndef EASYHAL_TIME_DEV_H_
#define EASYHAL_TIME_DEV_H_

#include <stdint.h>
#include <stdbool.h>

void time_dev_init(void);
void delay(uint32_t ms);
uint32_t millis(void);

#endif /* EASYHAL_TIME_DEV_H_ */
