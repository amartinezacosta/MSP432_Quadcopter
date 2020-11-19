#include "UARTDEBUG.h"
#include "EasyHal/uart_dev.h"

#include <stdint.h>
#include <stdarg.h>

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

UART_Handle UARTDEBUG_handle;

void UARTDEBUG_init(uint32_t baudrate)
{
    UART_Params params;

    UART_Params_init(&params);
    params.baudRate = baudrate;
    params.readMode = UART_MODE_BLOCKING;
    params.writeMode = UART_MODE_BLOCKING;
    params.readTimeout = 100;
    params.writeTimeout = UART_WAIT_FOREVER;
    params.readEcho = UART_ECHO_OFF;
    params.dataLength = UART_LEN_8;

    UARTDEBUG_handle = UART_open(CONFIG_UART_0, &params);
    if (UARTDEBUG_handle == NULL)
    {
        while(1);
    }
}

inline void PrintChar(char c)
{
    UART_write(UARTDEBUG_handle, (uint8_t*)&c, 1);
}

void PrintString(char *string)
{
    /*while the string is not the null character*/
    while(*string)
    {
        PrintChar(*string);
        string++;
    }
}

/*Thanks! https://github.com/grbl/grbl/blob/master/grbl/print.c */
void PrintInteger(int integer)
{
    if(integer == 0)
    {
        PrintChar('0');
    }

    if(integer < 0)
    {
        PrintChar('-');
        integer = -integer;
    }

    char b[10];
    int digit = integer;

    uint8_t i = 0;
    while(digit)
    {
        b[i++] = digit % 10;
        digit /= 10;
    }

    while(i)
    {
        PrintChar('0' + b[i-1]);
        i--;
    }
}

/*Thanks! https://github.com/grbl/grbl/blob/master/grbl/print.c */
void PrintFloat(float n, uint8_t decimal_places)
{
    if (n < 0)
    {
        PrintChar('-');
      n = -n;
    }

    uint8_t decimals = decimal_places;
    while (decimals >= 2)
    {
      // Quickly convert values expected to be E0 to E-4.
      n *= 100;
      decimals -= 2;
    }

    if (decimals)
    {
      n *= 10;
    }

    // Add rounding factor. Ensures carryover through entire value.
    n += 0.5;

    // Generate digits backwards and store in string.
    uint8_t buf[32];
    uint8_t i = 0;
    uint32_t a = (long)n;
    // Place decimal point, even if decimal places are zero.
    buf[decimal_places] = '.';

    while(a > 0)
    {
    // Skip decimal point location
    if (i == decimal_places)
    {
        i++;
    }

    // Get digit
    buf[i++] = (a % 10) + '0';
    a /= 10;
    }

    while (i < decimal_places)
    {
      // Fill in zeros to decimal point for (n < 1)
     buf[i++] = '0';
    }

    // Fill in leading zero, if needed.
    if (i == decimal_places)
    {
    i++;
    buf[i++] = '0';
    }

    // Print the generated string.
    for (; i > 0; i--)
    {
        PrintChar(buf[i-1]);
    }
}

void UARTDEBUG_printf(const char *fs, ...)
{
    va_list valist;
    va_start(valist, fs);
    int i;
    char *s;
    float f;

    while(*fs)
    {
        if(*fs != '%')
        {
            PrintChar(*fs);
            fs++;
        }
        else
        {
            switch(*++fs)
            {
            case 'c':
                i = va_arg(valist, int);
                PrintChar((char)i);
                break;
            case 's':
                s = va_arg(valist, char*);
                PrintString(s);
                break;
            case 'i':
                i = va_arg(valist, int);
                PrintInteger(i);
                break;
            case 'f':
                f = va_arg(valist, double);
                PrintFloat(f, 3);
                break;
            }

            ++fs;
        }
    }
}

int UARTDEBUG_gets(char *str, int length, int tries, bool terminal)
{
    char c;
    uint32_t i = 0;

    while(tries)
    {
        while(UART_read(UARTDEBUG_handle, (uint8_t*)&c, 1))
        {
            if((c == '\n') || (c == '\r'))
            {
                //TODO: Check buffer overflow condition
                str[i++] = '\0';

                if(terminal)
                {
                    PrintString("\r\n");
                }

                return i;
            }
            /*Erase data from buffer if backspace is received*/
            else if((c == 127) || (c == 8))
            {
                if(i > 0)
                {
                    i--;
                }
                str[i] = 0;

                if(terminal)
                {
                    PrintString("\x1B[1D");
                    PrintString(" ");
                    PrintString("\x1B[1D");
                }
            }
            /*Store character on the buffer*/
            else
            {
                if(i < length)
                {
                    str[i++] = c;

                    if(terminal)
                    {
                        PrintChar(c);
                    }
                }
                else
                {
                    return length + 1;
                }
            }
        }

        tries--;
    }

    return i;
}
