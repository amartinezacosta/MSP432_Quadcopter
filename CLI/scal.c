#include "CLI.h"
#include "UARTDEBUG.h"
#include "MPU6050.h"
#include "QMC5883.h"

#include <string.h>

void scal_help(void)
{
    UARTDEBUG_printf("Calibrate accelerometer, gyroscope, magnetometer or all sensors at once\r\n");
    UARTDEBUG_printf("Usage: scal [options]\r\n");
    UARTDEBUG_printf("   options:\r\n");
    UARTDEBUG_printf("      -a \t Calibrate accelerometer\r\n");
    UARTDEBUG_printf("      -g \t Calibrate gyroscope\r\n");
    UARTDEBUG_printf("      -m \t Calibrate magnetometer\r\n");
    UARTDEBUG_printf("      -A \t Calibrate all sensors\r\n");
}

void scal_run_gyrosocope_calibration(void)
{
    //calibrate gyroscope
    int32_t gyro_offsets[] = {0, 0, 0};

    UARTDEBUG_printf("Calibrating gyrosocope\r\n");
    MPU6050_calibrate_gyroscope(gyro_offsets, 4);
    UARTDEBUG_printf("Done calibrating gyrosocope\r\n");
    UARTDEBUG_printf("Offsets gx = %i, gy = %i, gz = %i\r\n", gyro_offsets[0], gyro_offsets[1], gyro_offsets[2]);
}

void scal_run_accelerometer_calibration(void)
{
    //calibrate accelerometer
    int32_t accel_offsets[] = {0, 0, 0};

    UARTDEBUG_printf("Calibrating accelerometer\r\n");
    MPU6050_calibrate_accelerometer(accel_offsets, 4);
    UARTDEBUG_printf("Done calibrating accelerometer\r\n");
    UARTDEBUG_printf("Offsets ax = %i, ay = %i, az = %i\r\n", accel_offsets[0], accel_offsets[1], accel_offsets[2]);
}

void scal_run_magnetometer_calibration(void)
{
    //calibrate magnetometer
    int32_t mag_offsets[] = {0, 0, 0};

    UARTDEBUG_printf("Calibrating magnetometer\r\n");
    QMC5883_calibrate_magnetometer(mag_offsets);
    UARTDEBUG_printf("Done calibrating magnetometer\r\n");
    UARTDEBUG_printf("Offsets mx = %i, my = %i, mz = %i\r\n", mag_offsets[0], mag_offsets[1], mag_offsets[2]);
}

int scal(char **argv, int argc)
{
    int i;
    for(i = 0; i < argc; i++)
    {
        if(strcmp("-h", argv[i]) == 0)
        {
            scal_help();
        }

        else if(strcmp("-a", argv[i]) == 0)
        {
            scal_run_accelerometer_calibration();
        }

        else if(strcmp("-g", argv[i]) == 0)
        {
            scal_run_gyrosocope_calibration();
        }

        else if(strcmp("-m", argv[i]) == 0)
        {
            scal_run_magnetometer_calibration();
        }

        else if(strcmp("-A", argv[i]) == 0)
        {
            scal_run_accelerometer_calibration();
            scal_run_gyrosocope_calibration();
            scal_run_magnetometer_calibration();
        }

        //Unknown option
        else
        {
            UARTDEBUG_printf("scal error: unknown option \'%s\'\r\n", argv[i]);
        }
    }

    return CMD_OK;
}



