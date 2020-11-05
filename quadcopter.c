#include "MPU6050.h"
#include "QMC5883.h"
#include "BME280.h"
#include "GPS.h"
#include "ESC.h"
#include "RC.h"

#include "UARTDEBUG.h"
#include "telemetry.h"

#include "EasyHal/time_dev.h"

#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define BAROMETRIC_PRESSURE_HPA     1026       //hPa
#define DECLINATION_ANGLE           -0.26      //degrees

void *mainThread(void *arg0)
{
    time_dev_init();
    UARTDEBUG_init(115200);

    //1. Initialize hardware devices and sensors
    UARTDEBUG_printf("LOG:****UTEP Senior Project II Drone V1.0****\n");
    UARTDEBUG_printf("LOG:Initializing hardware devices\n");
    MPU6050_init(MPU6050_DEFAULT_ADDRESS);
    BME280_init();
    QMC5883_init();
    ESC_init();
    PPM_init();
    GPS_init();

    UARTDEBUG_printf("LOG:Hardware devices initialized! 1.MPU6050 2.BME280 3.QMC5883 4.ublox GPS 5.RC\n");

    //2. Initialize telemetry
    UARTDEBUG_printf("LOG:Initializing telemetry thread and queue\n");
    telemetry_init();
    UARTDEBUG_printf("LOG:telemetry thread running\n");

    //2. Arm and Calibrate ESCs
    UARTDEBUG_printf("LOG:Arming ESCs\n");
    //ESC_arm();
    UARTDEBUG_printf("LOG:ESCs Armed!\n");
    UARTDEBUG_printf("LOG:Calibrating ESCs\n");
    //ESC_calibrate();
    UARTDEBUG_printf("LOG:ESCs calibrated!\n");

    //3. Calibrate gyroscope and Accelerometer
    int32_t accel_offset[3] = {0, 0, 0};
    int32_t gyro_offset[3] = {0, 0, 0};

    UARTDEBUG_printf("LOG:Calibrating gyroscope, DO NOT MOVE BOARD!\n");
    //MPU6050_calibrate_gyroscope(gyro_offset, 4);
    UARTDEBUG_printf("LOG:Done calibrating gyroscope, offsets = %i, %i, %i\n", gyro_offset[0], gyro_offset[1], gyro_offset[2]);

    UARTDEBUG_printf("LOG:Calibrating accelerometer, PLACE BOARD FLAT ON THE TABLE!\n");
    //MPU6050_calibrate_accelerometer(accel_offset, 4);
    UARTDEBUG_printf("LOG:Done calibrating accelerometer, offsets = %i, %i, %i\n", accel_offset[0], accel_offset[1], accel_offset[2]);

    //4. All done! Start control loop!
    UARTDEBUG_printf("LOG:Everything is done! Drone control loop is about to start!\n");

    //IMU and magnetometer data
    int16_t raw_accel[3];
    int16_t raw_gyro[3];
    int16_t raw_mag[3];

    float accel[3];
    float gyro[3];
    float mag[3];

    float accel_or[2] = {0.0, 0.0};
    float mag_or = 0.0;

    float orientation[3] = {0.0, 0.0, 0.0};

    //GPS data
    float location[2] = {0.0, 0.0};
    uint32_t satellites = 0;

    //Pressure sensor data
    float pressure = 0.0;
    float altitude = 0.0;

    //Timing data
    uint32_t start;
    float dt = 0.0;
    float t = 0.0;

    //RC data
    uint32_t channels[8];

    //TODO: hold here for commands?
    while(1)
    {
        start = millis();

        //1. Obtain IMU data, GPS, Pressure data and RC data
        MPU6050_raw_accelerometer(raw_accel);
        MPU6050_raw_gyroscope(raw_gyro);
        QMC5883_raw_magnetometer(raw_mag);
        PPM_channels(channels);
        GPS_read(location, &satellites);

        pressure = BME280_pressure();
        altitude = BME280_altitude(BAROMETRIC_PRESSURE_HPA);

        //a.Gyro raw reading, assuming +-250dps
        raw_gyro[0] -= gyro_offset[0];
        raw_gyro[1] -= gyro_offset[1];
        raw_gyro[2] -= gyro_offset[2];

        gyro[0] = ((float)raw_gyro[0])*0.00763;
        gyro[1] = ((float)raw_gyro[1])*0.00763;
        gyro[2] = ((float)raw_gyro[2])*0.00763;

        //b.Accelerometer reading, assuming +-2g
        raw_accel[0] -= accel_offset[0];
        raw_accel[0] -= accel_offset[0];
        raw_accel[0] -= accel_offset[0];

        accel[0] = ((float)raw_accel[0])*0.00006103;
        accel[1] = ((float)raw_accel[1])*0.00006103;
        accel[2] = ((float)raw_accel[2])*0.00006103;

        //c. Magnetometer reading, assuming +-8G
        mag[0] = ((float)raw_mag[0])*0.0003333;
        mag[1] = ((float)raw_mag[1])*0.0003333;
        mag[2] = ((float)raw_mag[2])*0.0003333;

        //2. Compute angle orientation based form accelerometer, gyroscope and magnetometer readings
        //a. Gyroscope angles
        orientation[0] += gyro[0]*dt;
        orientation[1] += gyro[1]*dt;
        orientation[2] += gyro[2]*dt;

        //b. Accelerometer angles
        float a_mag = sqrt((raw_accel[0]*raw_accel[0]) + (raw_accel[1]*raw_accel[1]) + (raw_accel[2]*raw_accel[2]));
        accel_or[0] = asin((float)raw_accel[1]/a_mag) * 57.296;
        accel_or[1] = asin((float)raw_accel[0]/a_mag) * 57.296;

        //c.Magnetometer Angles
        float pitch_rads = orientation[0] * 0.0174533;
        float roll_rads = orientation[1] * -0.0174533;

        float xh = ((float)mag[0])*cos(pitch_rads) + ((float)mag[1])*sin(roll_rads)*sin(pitch_rads) - ((float)mag[2])*cos(roll_rads)*sin(pitch_rads);
        float yh = ((float)mag[1])*cos(roll_rads) + ((float)mag[2])*sin(roll_rads);
        mag_or = atan2(yh, xh) * 57.296;

        mag_or += DECLINATION_ANGLE;

        //3. Use a data fusion algorithm to obtain final angle orientations. This controller uses complimentary filters
        orientation[0] = orientation[0] * 0.98 + accel_or[0] * 0.02;
        orientation[1] = orientation[1] * 0.98 + accel_or[1] * 0.02;
        orientation[2] = mag_or; //orientation[2] * 0.5 + mag_or * 0.5;

        //4. PID controller

        //5. Send telemetry data
        telemetry_send(orientation, gyro, accel, mag, location, satellites, pressure, altitude, t, dt);

        //Debug data here
        //UARTDEBUG_printf("%f,%c,%f,%c,%i\n\r", location[0], direction[0], location[1], direction[1], satellites);

        //usleep(2000);
        dt = (millis() - start)/1e3;
        t += dt;
    }
}

