#include "MPU6050.h"
#include "QMC5883.h"
#include "BME280.h"
#include "GPS.h"
#include "ESC.h"
#include "RC.h"

#include "UARTDEBUG.h"
#include "telemetry.h"

#include "EasyHal/time_dev.h"

#include <math.h>

#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

float compass_heading(float mx, float my, float mz, float *pitch, float *roll)
{
    float pitch_rad = -*pitch*0.0174533;
    float roll_rad = -*roll*0.0174533;

    float hx = mx*cos(pitch_rad) + my*sin(roll_rad)*sin(pitch_rad) - mz*cos(roll_rad)*sin(pitch_rad);
    float hy = my*cos(roll_rad) + mz*sin(roll_rad);

    float heading = atan2(hy, hx)*57.296;

    if(hy < 0 ) heading = 180 + (180 + atan2(hy, hx)*57.296);
    else        heading = atan2(hy, hx)*57.296;


    return heading;
}

void accelerometer_angles(float ax, float ay, float az, float *pitch, float *roll)
{
    float accel_magnitude = sqrt(ax*ax + ay*ay + az*az);
    *pitch = asin(ay/accel_magnitude)*57.296;
    *roll = -asin(ax/accel_magnitude)*57.296;
}

void gyroscope_angles(float gx, float gy, float gz, float *pitch, float *roll, float *yaw, float dt)
{
    *pitch += gx*dt;
    *roll += gy*dt;
    *yaw += gz*dt;

    *pitch -= *roll*sin(gz*0.0174533*dt);
    *roll +=  *pitch*sin(gz*0.0174533*dt);
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

    //Timing data
    uint32_t start;
    float dt = 0.0;
    float t = 0.0;

    //0. For timing and debugging
    time_dev_init();
    UARTDEBUG_init(115200);

    //Wait for everything to settle
    delay(100);

    GPIO_write(CONFIG_GPIO_0, 1);
    GPIO_write(CONFIG_GPIO_1, 1);

    //1. Initialize hardware devices and sensors
    MPU6050_init(MPU6050_DEFAULT_ADDRESS);
    QMC5883_init();

    //2. Calibrate Sensors
    MPU6050_calibrate_gyroscope(gyro_offset, 4);
    MPU6050_calibrate_accelerometer(accel_offset, 4);

    //3. Use accelerometer and magnetometer to set initial frame of reference
    MPU6050_accelerometer(accel, accel_offset);
    QMC5883_magnetometer(mag, mag_offset);

    accelerometer_angles(accel[0], accel[1], accel[2], &accel_pitch, &accel_roll);
    mag_yaw = compass_heading(mag[0], mag[1], mag[2], &pitch, &roll);

    pitch = accel_pitch;
    roll = accel_roll;
    yaw = mag_yaw;

    GPIO_write(CONFIG_GPIO_0, 0);
    GPIO_write(CONFIG_GPIO_1, 0);

    while(1)
    {
        start = millis();

        //1. Obtain IMU data, GPS, Pressure data and RC data
        MPU6050_gyroscope(gyro, gyro_offset);
        MPU6050_accelerometer(accel, accel_offset);
        QMC5883_magnetometer(mag, mag_offset);

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

        //6. Debug data here
        //UARTDEBUG_printf("%f,%f,%f,%f\n", pitch, roll, yaw, dt);

        dt = (millis() - start)/1e3;
        t += dt;
    }
}

