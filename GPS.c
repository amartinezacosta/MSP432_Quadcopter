#include "GPS.h"
#include "EasyHal/uart_dev.h"
#include "EasyHal/time_dev.h"

void GPS_init(void)
{
    uart_dev_init(UART1, 9600);
}

uint32_t GPS_read(char *gps_data, uint32_t length)
{
    char c;
    uint32_t i = 0;

    //Read until new line received
    while(1)
    {
        uart_dev_read(UART1, (uint8_t*)&c, 1);

       /*put a '\n' and '\r' if it fits on the buffer*/
       if((c == '\n') || (c == '\r'))
       {
           //Save carriage return and newline
           if((i + 3) < length)
           {
               gps_data[i++] = c;
               uart_dev_read(UART1, (uint8_t*)&c, 1);
               gps_data[i++] = 0;
           }

           return i;
       }

       /*Store character on the buffer*/
       else
       {
           if(i < length)
           {
               gps_data[i++] = c;
           }
           else
           {
               return length + 1;
           }
       }

    }
}


