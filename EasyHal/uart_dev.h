#ifndef UART_DEV_H_
#define UART_DEV_H_

#include <stdint.h>
#include <stdbool.h>

#define UART_COUNT  2

#define UART0   0
#define UART1   1

#define UART_BAUD_9600          9600
#define UART_BAUD_115200        115200

void uart_dev_init(uint32_t index, uint32_t baudrate);
void uart_dev_write(uint32_t index, uint8_t *data, uint32_t size);
void uart_dev_read(uint32_t index, uint8_t *data, uint32_t size);


#endif /* UART_DEV_H_ */
