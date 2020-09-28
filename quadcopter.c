#include "MPU6050.h"
#include "QMC5883.h"
#include "ESC.h"
#include "UARTDEBUG.h"

#include "EasyHal/time_dev.h"

void *mainThread(void *arg0)
{
    time_dev_init();

    UARTDEBUG_init(9600);
    MPU6050_init(MPU6050_DEFAULT_ADDRESS);
    QMC5883_init();
    ESC_init();

    ESC_arm();
    ESC_calibrate();

    int16_t raw_accel[3];
    int16_t raw_gyro[3];
    int16_t raw_mag[3];

    uint32_t start;
    float dt = 0.0;

    while(1)
    {
        start = millis();

        MPU6050_raw_accelerometer(raw_accel);
        MPU6050_raw_gyroscope(raw_gyro);
        QMC5883_raw_magnetometer(raw_mag);

        UARTDEBUG_printf("ax = %i, ay = %i, az = %i, ", raw_accel[0], raw_accel[1], raw_accel[2]);
        UARTDEBUG_printf("gx = %i, gy = %f, gz = %i, ", raw_gyro[0], raw_gyro[1], raw_gyro[2]);
        UARTDEBUG_printf("mx = %i, my = %i, mz = %i, dt = %f\r\n", raw_mag[0], raw_mag[1], raw_mag[2], dt);

        dt = (millis() - start)/1e3;
    }
}


