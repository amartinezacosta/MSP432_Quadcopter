#include "telemetry.h"

#include <stdio.h>
#include <string.h>

#include "ti_drivers_config.h"
#include <ti/drivers/UART.h>

UART_Handle Telemetry_UART;
char telemetry_buffer[TELEMETRY_BUFFER_SIZE];

void telemetry_UART_RX_callback(UART_Handle handle, void *buf, uint32_t count);
void telemetry_UART_TX_callback(UART_Handle handle, void *buf, uint32_t count);

void telemetry_init(void)
{
    // Initialize UART parameters
    UART_Params params;
    UART_Params_init(&params);
    params.baudRate = 9600;
    params.readMode = UART_MODE_CALLBACK;
    params.readCallback = telemetry_UART_RX_callback;
    params.readDataMode = UART_DATA_TEXT;
    params.writeMode = UART_MODE_CALLBACK;
    params.writeCallback = telemetry_UART_TX_callback;
    params.writeDataMode = UART_DATA_BINARY;
    params.readEcho = UART_ECHO_OFF;

    // Open the UART
    Telemetry_UART = UART_open(CONFIG_UART_1, &params);
}


void telemetry_UART_RX_callback(UART_Handle handle, void *buf, uint32_t count)
{
    //Process telemetry packet
}

void telemetry_UART_TX_callback(UART_Handle handle, void *buf, uint32_t count)
{
    //Data has been sent, do nothing
}

void telemetry_char(char *buffer, char c, uint32_t *index)
{
    buffer[*index] = c;
    *index = *index + 1;
}

void telemetry_string(char *buffer, char *string, uint32_t *index)
{
    /*while the string is not the null character*/
    while(*string)
    {
        telemetry_char(buffer, *string, index);
        string++;
    }
}

/*Thanks! https://github.com/grbl/grbl/blob/master/grbl/print.c */
void telemetry_integer(char *buffer, int integer, uint32_t *index)
{
    if(integer == 0)
    {
        telemetry_char(buffer, '0', index);
    }

    if(integer < 0)
    {
        telemetry_char(buffer, '-', index);
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
        telemetry_char(buffer, '0' + b[i-1], index);
        i--;
    }
}

/*Thanks! https://github.com/grbl/grbl/blob/master/grbl/print.c */
void telemetry_float(char *buffer, float n, uint8_t decimal_places, uint32_t *index)
{
    if (n < 0)
    {
        telemetry_char(buffer, '-', index);
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
        telemetry_char(buffer, buf[i-1], index);
    }
}

void telemetry_sprintf(char *buffer, const char *fs, ...)
{
    va_list valist;
    va_start(valist, fs);
    int i;
    char *s;
    float f;
    uint32_t index = 0;

    while(*fs)
    {
        if(*fs != '%')
        {
            telemetry_char(buffer, *fs, &index);
            fs++;
        }
        else
        {
            switch(*++fs)
            {
            case 'c':
                i = va_arg(valist, int);
                telemetry_char(buffer, (char)i, &index);
                break;
            case 's':
                s = va_arg(valist, char*);
                telemetry_string(buffer, s, &index);
                break;
            case 'i':
                i = va_arg(valist, int);
                telemetry_integer(buffer, i, &index);
                break;
            case 'f':
                f = va_arg(valist, double);
                telemetry_float(buffer, f, 3, &index);
                break;
            }

            ++fs;
        }
    }

    buffer[index] = '\0';
}

/*Telemetry packet protocol
 *
 * Every telemetry packet starts with a 'TEL:' heading and it is sent using ASCII characters and is terminated with a new line character '\n'
 * SIZE = 20 strings of variable length
 *
 *
 * TEL:pitch,roll,yaw,gyro_x,gyro_y,gyro_z,accel_x,accel_y,accel_z,mag_x,mag_y,mag_z,gps_lat,gps_long,gps_elevation,gps_satellites,pressure,altitude,dt,time
 *
 * */
void telemetry_send(float *angle, float *gyro, float *accel, float *mag, float *location, uint32_t satellites, float pressure, float altitude, float dt, float t)
{
    telemetry_sprintf(telemetry_buffer, "TEL:%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%i,%f,%f,%f,%f\n", angle[0], angle[1], angle[2],
                                                                                                             gyro[0], gyro[1], gyro[2],
                                                                                                             accel[0], accel[1], accel[2],
                                                                                                             mag[0], mag[1], mag[2],
                                                                                                             location[0], location[1], location[2],
                                                                                                             satellites, pressure, altitude,
                                                                                                             dt, t);

    UART_write(Telemetry_UART, telemetry_buffer, strlen(telemetry_buffer));
}

/*
void *telemetry_thread(void *arg0);

mqd_t mq;

void telemetry_init(void)
{
    //Setup queue
    struct mq_attr attr;

    //Setup queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = TELEMETRY_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(tel_packet_t);
    attr.mq_curmsgs = 0;

    //Create queue
    mq = mq_open("TELEMETRY_QUEUE", O_CREAT | O_NONBLOCK | O_RDWR, 0644, &attr);

    //Create telemetry thread
    pthread_t tid;
    pthread_attr_t      attrs;
    struct sched_param  priParam;


    pthread_attr_init(&attrs);


    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&attrs, &priParam);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attrs, 1024);

    pthread_create(&tid, NULL, telemetry_thread, NULL);
}

void telemetry_queue(float *angle, float *gyro, float *accel, float *mag, float *location, uint32_t satellites, float pressure, float altitude, float dt, float t)
{
    //Create telemetry packet
    tel_packet_t packet;

    uint32_t i;
    for(i = 0; i < 3; i++)
    {
        angle[i] = angle[i];
        packet.gyro[i] = gyro[i];
        packet.accel[i] = accel[i];
        packet.mag[i] = mag[i];
        packet.location[i] = location[i];
    }

    packet.satellites = satellites;
    packet.pressure = pressure;
    packet.altitude = altitude;
    packet.dt = dt;
    packet.time = t;

    //Queue telemetry packet
    mq_send(mq, (char*)&packet, sizeof(tel_packet_t), NULL);
}
void *telemetry_thread(void *arg0)
{
    tel_packet_t packet;
    int status;
    char telemetry_data[512];

    while(1)
    {
        status = mq_receive(mq, (char*)&packet, sizeof(tel_packet_t), NULL);
        if(status > 0)
        {
            UARTDEBUG_printf("TEL:%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%i,%f,%f,%f,%f\n", packet.angle[0], packet.angle[1], packet.angle[2],
                                                                                                  packet.gyro[0], packet.gyro[1], packet.gyro[2],
                                                                                                  packet.accel[0], packet.accel[1], packet.accel[2],
                                                                                                  packet.mag[0], packet.mag[1], packet.mag[2],
                                                                                                  packet.location[0], packet.location[1], packet.location[2],
                                                                                                  packet.satellites, packet.pressure, packet.altitude,
                                                                                                  packet.dt, packet.time);

        }

        usleep(1000);

    }
}
*/
