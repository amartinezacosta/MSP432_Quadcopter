#ifndef I2C_DEV_H_
#define I2C_DEV_H_

#include <stdint.h>
#include <stdbool.h>

#define I2C_DEV_100KHZ      100000
#define I2C_DEV_400KHZ      400000
#define I2C_DEV_1MHZ        1000000

void i2c_dev_init(uint32_t i2c_clock);
bool i2c_dev_write(uint8_t slave_address, uint8_t *data, uint32_t size);
bool i2c_dev_read(uint8_t slave_address, uint8_t reg, uint8_t *data, uint32_t size);


#endif /* I2C_DEV_H_ */
