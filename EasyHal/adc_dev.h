#ifndef EASYHAL_ADC_DEV_H_
#define EASYHAL_ADC_DEV_H_

#define ADC0    0
#define ADC1    1
#define ADC2    2
#define ADC3    3

#define ADC_COUNT   4

#include <stdint.h>
#include <stdbool.h>

void adc_dev_init(uint32_t index);
uint16_t adc_dev_read(uint32_t index);

#endif /* EASYHAL_ADC_DEV_H_ */
