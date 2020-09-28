#ifndef EASYHAL_SPI_DEV_H_
#define EASYHAL_SPI_DEV_H_

#include <stdint.h>
#include <stdbool.h>

#define SPI_DEV_100KHZ      100000
#define SPI_DEV_1MHZ        1000000
#define SPI_DEV_3MHZ        3000000
#define SPI_DEV_12MHZ       12000000
#define SPI_DEV_24MHZ       24000000

#define CS_HIGH     1
#define CS_LOW      0

void spi_dev_init(uint32_t spi_clock);
void spi_dev_write(uint8_t *data, uint32_t size);
void spi_dev_read(uint8_t *data, uint32_t size);
void spi_dev_cs(uint8_t state);

#endif /* EASYHAL_SPI_DEV_H_ */
