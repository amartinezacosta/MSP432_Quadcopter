#include "MPU6050.h"
#include "QMC5883.h"
#include "BME280.h"
#include "GPS.h"
#include "ESC.h"
#include "RC.h"
#include "IMU.h"
#include "LED.h"
#include "CLI.h"

#include "UARTDEBUG.h"
#include "telemetry.h"

#include "EasyHal/time_dev.h"

#include <string.h>

int cli_sensor_calibrate(char **argv, int argc)
{
    return CMD_OK;
}

int cli_sensor_check(char **argv, int argc)
{
    return CMD_OK;
}

int cli_sensor_read(char **argv, int argc)
{
    return CMD_OK;
}

int cli_sensor_calibrate_read(char **argv, int argc)
{
    return CMD_OK;
}

int cli_imu_read(char **argv, int argc)
{
    return CMD_OK;
}

int cli_test_led(char **argv, int argc)
{
    return CMD_OK;
}

int cli_vbat_read(char **argv, int argc)
{
    return CMD_OK;
}

int cli_rc_read(char **argv, int argc)
{
    return CMD_OK;
}

int cli_motor_check(char **argv, int argc)
{
    return CMD_OK;
}

void *mainThread(void *arg0)
{
    //Calibration data
    int32_t gyro_offset[3] = {0, 0, 0};
    int32_t accel_offset[3] = {0, 0, 0};
    int32_t mag_offset[3] = {0, 0, 0};

    //Gyrosocope, accelerometer and magnetometer data
    float gyro[3];
    float accel[3];
    float mag[3];
    float accel_pitch, accel_roll;
    float mag_yaw;
    float pitch = 0.0, roll = 0.0, yaw = 0.0;

    //RC data
    uint32_t channels[8];

    //GPS data
    float location[2];
    uint32_t satellites;

    //Pressure altitude data
    float pressure;
    float altitude;

    //Timing data
    uint32_t start;
    float dt = 0.0;
    float t = 0.0;

    //Debugging/CLI data
    char c;

    //Wait for everything to settle
    delay(100);

    //0. For timing and debugging
    time_dev_init();
    UARTDEBUG_init(115200);

    //1. Initialize hardware devices and sensors
    MPU6050_init(MPU6050_DEFAULT_ADDRESS);
    QMC5883_init();
    telemetry_init();
    GPS_init();
    BME280_init();
    ESC_init();
    PPM_init();

    LED_sequence_both(Hz_2, 4);

    //2. Enter command line application if requested
    if(UARTDEBUG_gets(&c, 1, 10, false))
    {
        LED_sequence_both(Hz_15, 10);
        cli();
    }

    //2. Calibrate Sensors
    LED_sequence(RED_LED, Hz_10, 10);
    MPU6050_calibrate_gyroscope(gyro_offset, 4);
    MPU6050_calibrate_accelerometer(accel_offset, 4);

    //3. Use accelerometer and magnetometer to set initial frame of reference
    LED_sequence(RED_LED, Hz_20, 20);
    MPU6050_accelerometer(accel, accel_offset);
    QMC5883_magnetometer(mag, mag_offset);

    accelerometer_angles(accel[0], accel[1], accel[2], &accel_pitch, &accel_roll);
    mag_yaw = compass_heading(mag[0], mag[1], mag[2], &pitch, &roll);

    pitch = accel_pitch;
    roll = accel_roll;
    yaw = mag_yaw;

    LED_sequence(RED_LED, Hz_30, 30);
    ESC_calibrate();
    ESC_arm();

    LED_solid(RED_LED, 0);
    LED_solid(GREEN_LED, 1);

    while(1)
    {
        start = millis();

        //1. Obtain IMU data, GPS, Pressure data and RC data
        MPU6050_gyroscope(gyro, gyro_offset);
        MPU6050_accelerometer(accel, accel_offset);
        QMC5883_magnetometer(mag, mag_offset);
        GPS_read(location, &satellites);
        PPM_channels(channels);

        pressure = BME280_pressure();
        altitude = BME280_altitude(BAROMETRIC_PRESSURE_HPA);

        //a. Compute angle using gyro data
        gyroscope_angles(gyro[0], gyro[1], gyro[2], &pitch, &roll, &yaw, dt);

        //b. Compute angle using accelerometer
        accelerometer_angles(accel[0], accel[1], accel[2], &accel_pitch, &accel_roll);

        //c. Yaw angle from compass
        mag_yaw = compass_heading(mag[0], mag[1], mag[2], &pitch, &roll);

        //c. complementary filter
        pitch = 0.98 * pitch + 0.02 * accel_pitch;
        roll = 0.98 * roll + 0.02 * accel_roll;
        yaw = 0.98 * yaw + 0.02 * mag_yaw;

        //Check flight mode here!
        ESC_speed(ESC0, 14000 + channels[0]);
        ESC_speed(ESC1, 14000 + channels[0]);
        ESC_speed(ESC2, 14000 + channels[1]);
        ESC_speed(ESC3, 14000 + channels[1]);

        //6. telemetry data here
        float angles[] = {pitch, roll, yaw};
        telemetry_send(angles, gyro, accel, mag, location, satellites, pressure, altitude, dt, t);

        dt = (millis() - start)/1e3;
        t += dt;
    }
}

