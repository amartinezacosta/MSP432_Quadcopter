#include "CLI.h"
#include "MPU6050.h"
#include "BME280.h"
#include "QMC5883.h"
#include "UARTDEBUG.h"

#include <string.h>
#include <stdlib.h>

#define SREAD_MAX_SAMPLES   2048

void sread_run_help(void)
{
    int max_samples = SREAD_MAX_SAMPLES;

    UARTDEBUG_printf("Read raw accelerometer, gyroscope or magnetometer data. Maximum number of samples is %i\r\n", max_samples);
    UARTDEBUG_printf("Usage: sread [options] samples\r\n");
    UARTDEBUG_printf("   options:\r\n");
    UARTDEBUG_printf("      -a \t Read raw accelerometer data\r\n");
    UARTDEBUG_printf("      -g \t Read raw gyroscope data\r\n");
    UARTDEBUG_printf("      -m \t Read raw magnetometer data\r\n");
    UARTDEBUG_printf("      -p \t Read raw pressure data\r\n");
}

bool sread_run_gyroscope_read(char *arg)
{
    if(arg == NULL)
    {
        UARTDEBUG_printf("sread error: number of samples must be provided\r\n");
        return false;
    }

    int n = atoi(arg);

    if((n > SREAD_MAX_SAMPLES) || (n < 0))
    {
        UARTDEBUG_printf("sread error: incorrect number of samples\r\n");
        return true;
    }

    int i;
    float gyro[3];
    int32_t gyro_offsets[3] = {0, 0, 0};

    UARTDEBUG_printf("Gyrosocope raw data, samples = %i\r\n", n);
    for(i = 0; i < n; i++)
    {
        MPU6050_gyroscope(gyro, gyro_offsets);
        UARTDEBUG_printf("%f,%f,%f\r\n", gyro[0], gyro[1], gyro[2]);
    }

    return true;
}

bool sread_run_accelerometer_read(char *arg)
{
    if(arg == NULL)
    {
        UARTDEBUG_printf("sread error: number of samples must be provided\r\n");
        return false;
    }

    int n = atoi(arg);

    if((n > SREAD_MAX_SAMPLES) || (n < 0))
    {
        UARTDEBUG_printf("sread error: incorrect number of samples\r\n");
        return true;
    }

    int i;
    float accel[3];
    int32_t accel_offsets[3] = {0, 0, 0};

    UARTDEBUG_printf("Accelerometer raw data, samples = %i\r\n", n);
    for(i = 0; i < n; i++)
    {
        MPU6050_accelerometer(accel, accel_offsets);
        UARTDEBUG_printf("%f,%f,%f\r\n", accel[0], accel[1], accel[2]);
    }

    return true;
}

bool sread_run_magnetometer_read(char *arg)
{
    if(arg == NULL)
    {
        UARTDEBUG_printf("sread error: number of samples must be provided\r\n");
        return false;
    }

    int n = atoi(arg);

    if((n > SREAD_MAX_SAMPLES) || (n < 0))
    {
        UARTDEBUG_printf("sread error: incorrect number of samples\r\n");
        return true;
    }

    int i;
    float mag[3];
    int32_t mag_offsets[3] = {0, 0, 0};

    UARTDEBUG_printf("Magnetometer raw data, samples = %i\r\n", n);
    for(i = 0; i < n; i++)
    {
        QMC5883_magnetometer(mag, mag_offsets);
        UARTDEBUG_printf("%f,%f,%f\r\n", mag[0], mag[1], mag[2]);
    }

    return true;
}

bool sread_run_pressure_read(char *arg)
{
    if(arg == NULL)
    {
        UARTDEBUG_printf("sread error: number of samples must be provided\r\n");
        return false;
    }

    int n = atoi(arg);

    if((n > SREAD_MAX_SAMPLES) || (n < 0))
    {
        UARTDEBUG_printf("sread error: incorrect number of samples\r\n");
        return true;
    }

    int i;
    float pressure;

    UARTDEBUG_printf("Pressure raw data, samples = %i\r\n", n);
    for(i = 0; i < n; i++)
    {
        pressure = BME280_pressure();
        UARTDEBUG_printf("%f\r\n", pressure);
    }

    return true;
}

int sread(char **argv, int argc)
{
    int i;
    for(i = 0; i < argc; i++)
    {
        //Help option
        if(strcmp("-h", argv[i]) == 0)
        {
            sread_run_help();
        }

        //Accelerometer option
        else if(strcmp("-a", argv[i]) == 0)
        {
            if(sread_run_accelerometer_read(argv[i + 1])) i++;
        }

        //Gyrosocope option
        else if(strcmp("-g", argv[i]) == 0)
        {
            if(sread_run_gyroscope_read(argv[i + 1])) i++;
        }

        //Magnetometer option
        else if(strcmp("-m", argv[i]) == 0)
        {
            if(sread_run_magnetometer_read(argv[i + 1])) i++;
        }

        //Pressure option
        else if(strcmp("-p", argv[i]) == 0)
        {
            if(sread_run_pressure_read(argv[i + 1])) i++;
        }

        //Unknown option
        else
        {
            UARTDEBUG_printf("sread error: unknown option \'%s\'\r\n", argv[i]);
        }
    }

    return CMD_OK;
}



