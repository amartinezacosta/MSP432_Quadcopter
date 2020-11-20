#include "CLI.h"
#include "MPU6050.h"
#include "BME280.h"
#include "QMC5883.h"
#include "UARTDEBUG.h"

#include <string.h>
#include <stdlib.h>

int sread(char **argv, int argc)
{
    int i;
    for(i = 0; i < argc; i++)
    {
        //Help option
        if(strcmp("-h", argv[i]) == 0)
        {
            UARTDEBUG_printf("Read raw accelerometer, gyroscope or magnetometer data. Maximum number of samples is 1024\r\n");
            UARTDEBUG_printf("Usage: sread [options] samples\r\n");
            UARTDEBUG_printf("   options:\r\n");
            UARTDEBUG_printf("      -a \t Read raw accelerometer data\r\n");
            UARTDEBUG_printf("      -g \t Read raw gyroscope data\r\n");
            UARTDEBUG_printf("      -m \t Read raw magnetometer data\r\n");
            UARTDEBUG_printf("      -p \t Read raw pressure data\r\n");
        }

        //Accelerometer option
        else if(strcmp("-a", argv[i]) == 0)
        {
            if(argv[i + 1] != NULL)
            {
                int samples = atoi(argv[i + 1]);

                if((samples > 1024) || (samples < 0))
                {
                    UARTDEBUG_printf("sread error: incorrect number of samples\r\n");
                }
                else
                {
                    int j;
                    float accel[3];
                    int32_t accel_offsets[] = {0, 0, 0};

                    UARTDEBUG_printf("Accelerometer raw data, samples = %i\r\n", samples);
                    for(j = 0; j < samples; j++)
                    {
                        MPU6050_accelerometer(accel, accel_offsets);
                        UARTDEBUG_printf("%f,%f,%f\r\n", accel[0], accel[1], accel[2]);
                    }
                }

                i++;
            }
            else
            {
                UARTDEBUG_printf("sread error: number of samples must be provided\r\n");
            }
        }

        //Gyrosocope option
        else if(strcmp("-g", argv[i]) == 0)
        {
            if(argv[i + 1] != NULL)
            {
                int samples = atoi(argv[i + 1]);

                if((samples > 1024) || (samples < 0))
                {
                    UARTDEBUG_printf("sread error: incorrect number of samples\r\n");
                }
                else
                {
                    int j;
                    float gyro[3];
                    int32_t gyro_offsets[] = {0, 0, 0};

                    UARTDEBUG_printf("Gyrosocope raw data, samples = %i\r\n", samples);
                    for(j = 0; j < samples; j++)
                    {
                        MPU6050_gyroscope(gyro, gyro_offsets);
                        UARTDEBUG_printf("%f,%f,%f\r\n", gyro[0], gyro[1], gyro[2]);
                    }
                }

                i++;
            }
            else
            {
                UARTDEBUG_printf("sread error: number of samples must be provided\r\n");
            }
        }

        //Unknown option
        else
        {
            UARTDEBUG_printf("sread error: unknown option \'%s\'\r\n", argv[i]);
        }
    }

    return CMD_OK;
}



