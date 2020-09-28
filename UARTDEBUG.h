#ifndef UART_DEBUGGER_H_
#define UART_DEBUGGER_H_

#include <stdbool.h>
#include <stdint.h>

/*
 * Function:  UARTDEBUG_init
 * --------------------
 *  Initialize UARTDEBUG communication channel
 *
 *  int fclock: clock speed for the UART module
 *  int buadrate: desired baudrate
 *
 *  returns: void
 */
void UARTDEBUG_init(uint32_t baudrate);

/*
 * Function:  UARTDEBUG_printf
 * --------------------
 *  Send formatted string to serial terminal
 *
 *  const char *fs: formatted string
 *  .. args: float, int, char*
 *
 *  example:
 *      char *str = "hello world"
 *      int my_int = 123;
 *      float pi = 3.145;
 *      UARTDEBUG_printf("HC test string=%s, int=%i, float=%f\r\n", str, my_int, pi);
 *
 *  returns: void
 */
void UARTDEBUG_printf(const char *fs, ...);

/*
 * Function:  UARTDEBUG_gets
 * --------------------
 *  Receive new line terminated string from serial terminal
 *
 *  char *str: buffer to store received string
 *  int length: size of buffer string
 *
 *  example:
 *      char str[32];
 *      int rcv;
 *      rcv = UARTDEBUG_gets(str, 32);
 *
 *  returns: int -> number of received characters on the string
 */
int UARTDEBUG_gets(char *str, int length, bool terminal);

#endif /* UART_DEBUGGER_H_ */
