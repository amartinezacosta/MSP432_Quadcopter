#include "GPS.h"
#include "EasyHal/uart_dev.h"
#include "EasyHal/time_dev.h"

void GPS_init(void)
{
    uart_dev_init(UART2, 9600);
}

uint32_t GPS_read(char *gps_data, uint32_t length)
{
    char c;
    uint32_t i = 0;

    //Read until new line received
    while(1)
    {
        //Read character
        uart_dev_read(UART2, (uint8_t*)&c, 1);

       if((c == '\n') || (c == '\r'))
       {
           //read carriage return
           uart_dev_read(UART2, (uint8_t*)&c, 1);

           //Process GPS packet data

           if((i + 3) < length)
           {
               gps_data[i++] = c;
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


