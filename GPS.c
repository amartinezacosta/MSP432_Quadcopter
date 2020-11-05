#include "GPS.h"
#include "UARTDEBUG.h"

#include <stdlib.h>
#include <string.h>

#include "ti_drivers_config.h"
#include <ti/drivers/UART.h>

//UART Handler
UART_Handle GPS_UART;

//UART buffer
uint32_t read_index = 0;
char message_buffer[256];
bool new_line = false;

//Token buffer
char *tokens[10];
uint32_t token_count = 0;

//Global GPS data
float latitude = 0.0;
float longitude = 0.0;
char latitude_direction = 'N';
char longitude_direction = 'W';
uint32_t satellite_count = 0;

void GPS_init(void)
{
    // Initialize UART parameters
    UART_Params params;
    UART_Params_init(&params);
    params.baudRate = 9600;
    params.readMode = UART_MODE_BLOCKING;
    //params.readCallback = GPS_UART_callback;
    params.readDataMode = UART_DATA_BINARY;
    params.writeMode = UART_MODE_BLOCKING;
    params.readEcho = UART_ECHO_OFF;
    params.writeTimeout = UART_WAIT_FOREVER;
    params.readTimeout = 0;

    // Open the UART
    GPS_UART = UART_open(CONFIG_UART_2, &params);
}

void GPS_read(float *location, uint32_t *satellites)
{
    char c;

    while(UART_read(GPS_UART, &c, 1))
    {
        if(c == '\n')
        {
            message_buffer[read_index-4] = '\0';
            read_index = 0;
            new_line = true;
        }
        else
        {
            message_buffer[read_index++] = c;
        }
    }


    if(new_line)
    {
        //UARTDEBUG_printf(message_buffer);

        char *token = strtok(message_buffer, ",");

        //Process $GPGGA data
        if(strcmp(token, "$GPGGA") == 0)
        {
            while(1)
            {
                token = strtok(NULL, ",");

                if(token != NULL)
                {
                    tokens[token_count++] = token;
                }
                else
                {
                    break;
                }
            }

            //If there are 11 tokens then there is a valid packet
            if(token_count > 4)
            {
                //Latitude is 2nd token
                latitude = GPS_parse_degrees(tokens[1]);
                //Latiutude direction
                latitude_direction = *tokens[2];
                latitude = (latitude_direction == 'S') ? -latitude : latitude;

                //Longitude is is 3rd token
                longitude = GPS_parse_degrees(tokens[3]);
                //Longitude direction
                longitude_direction = *tokens[4];
                longitude = (longitude_direction == 'W') ? -longitude : longitude;

                //Satellites
                satellite_count = atoi(tokens[6]);
            }
            else
            {
                //No valid data
            }


            /*
            uint32_t i;
            for(i = 0; i < token_count; i++)
            {
                UARTDEBUG_printf("token=%s ", tokens[i]);
            }

            UARTDEBUG_printf("\r\n");
            */
        }

        new_line = false;
        token_count = 0;
    }

    location[0] = latitude;
    location[1] = longitude;
    *satellites = satellite_count;
}

bool is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

//Taken from tinyGPS
float GPS_parse_degrees(char *data)
{
    char *p = data;
    unsigned long left_of_decimal = atol(data);
    unsigned long hundred1000ths_of_minute = (left_of_decimal % 100UL) * 100000UL;

    while(is_digit(*++p));

    if(*p == '.')
    {
        unsigned long mult = 10000;

        while(is_digit(*++p))
        {
            hundred1000ths_of_minute += mult * (*p - '0');
            mult /= 10;
        }
    }

    unsigned long int_result = (left_of_decimal / 100) * 1000000 + (hundred1000ths_of_minute + 3) / 6;

    return ((float)int_result/1000000.0);
}

//Convert a string ddmm.mmmm to degress
float GPS_latitude_degrees(char *data)
{
    int32_t latitude;

    latitude =  ((int32_t)data[2] - 48) * 10000000;
    latitude += ((int32_t)data[3] - 48) * 1000000;
    latitude += ((int32_t)data[5] - 48) * 100000;
    latitude += ((int32_t)data[6] - 48) * 10000;
    latitude += ((int32_t)data[7] - 48) * 1000;
    latitude += ((int32_t)data[8] - 48) * 100;
    latitude += ((int32_t)data[9] - 48) * 10;
    latitude /= 6;
    latitude += ((int32_t)data[0] - 48) * 100000000;
    latitude += ((int32_t)data[1] - 48) * 10000000;
    latitude /= 10;

    return ((float)latitude/1000000.0);
}

//Convert a string dddmm.mmmm to degress
float GPS_longitude_degrees(char *data)
{
    int32_t longitude;

    longitude =  ((int32_t)data[2] - 48) * 10000000;
    longitude += ((int32_t)data[3] - 48) * 1000000;
    longitude += ((int32_t)data[5] - 48) * 100000;
    longitude += ((int32_t)data[6] - 48) * 10000;
    longitude += ((int32_t)data[7] - 48) * 1000;
    longitude += ((int32_t)data[8] - 48) * 100;
    longitude += ((int32_t)data[9] - 48) * 10;
    longitude /= 6;
    longitude += ((int32_t)data[0] - 48) * 1000000000;
    longitude += ((int32_t)data[1] - 48) * 100000000;
    longitude += ((int32_t)data[2] - 48) * 10000000;
    longitude /= 10;

    return ((float)longitude/1000000.0);
}
