#include "MPU9250.h"
#include "EasyHal/i2c_dev.h"
#include "EasyHal/time_dev.h"

#include <math.h>

//Based on Kriswner and Betaflight

/*CALIBRATION DATA*/

/*BIAS DATA*/


/*RESOLUTION DATA*/
uint8_t accel_res;
uint8_t gyro_res;
uint8_t mag_res;

/*SCALE DATA*/
static float mag_scale;
static float accel_sca;
static float gyro_sca;

void MPU9250_init(void)
{
    i2c_dev_init(I2C_DEV_400KHZ);
    MPU9250_write_byte(PWR_MGMT_1, 0x00);
    delay(100);

    //Enable AK8963 magnetometer
    MPU9250_write_byte(INT_PIN_CFG, 0x02);

    //Reset value is +-250dps
    gyro_sca = 250.0f/32678.0f;
    gyro_res = GFS_250DPS;

    //Reset value is +-2g
    accel_sca = 2.0f/32768.0f;
    accel_res = AFS_2G;

    //Reset value
    mag_res = MFS_16BITS;
    mag_scale = 10.0f*4912.0f/32760.f;

    AK8963_init();
}

void AK8963_init(void)
{
    AK8963_write_byte(AK8963_CNTL, MFS_16BITS << 4 | 0x02);
    delay(10);
}

uint8_t MPU9250_who(void)
{
    uint8_t c;
    i2c_dev_read(MPU9250_1_ADDRESS, WHO_AM_I_MPU9250, &c, 1);

    return c;
}

uint8_t AK8963_who(void)
{
    uint8_t c;
    i2c_dev_read(AK8963_ADDRESS, WHO_AM_I_AK8963, &c, 1);

    return c;
}

void MPU9250_reset(void)
{
    MPU9250_write_byte(PWR_MGMT_1, 0x80);
    delay(100);
}

void MPU9250_raw_accelerometer(int16_t *accel_data)
{
    uint8_t raw_data[6];
    i2c_dev_read(MPU9250_1_ADDRESS, ACCEL_XOUT_H, raw_data, 6);

    accel_data[0] = (((int16_t)raw_data[0]) << 8) | raw_data[1];
    accel_data[1] = (((int16_t)raw_data[2]) << 8) | raw_data[3];
    accel_data[2] = (((int16_t)raw_data[4]) << 8) | raw_data[5];
}

void MPU9250_raw_gyroscope(int16_t *gyro_data)
{
    uint8_t raw_data[6];
    i2c_dev_read(MPU9250_1_ADDRESS, GYRO_XOUT_H, raw_data, 6);

    gyro_data[0] = (((int16_t)raw_data[0]) << 8) | raw_data[1];
    gyro_data[1] = (((int16_t)raw_data[2]) << 8) | raw_data[3];
    gyro_data[2] = (((int16_t)raw_data[4]) << 8) | raw_data[5];
}

void AK8963_raw_magnetometer(int16_t *mag_data)
{
    uint8_t raw_data[7];

    i2c_dev_read(AK8963_ADDRESS, AK8963_XOUT_L, raw_data, 7);
    if(!(raw_data[6] & 0x08))
    {
        mag_data[0] = (((int16_t)raw_data[1]) << 8) | raw_data[0];
        mag_data[1] = (((int16_t)raw_data[3]) << 8) | raw_data[2];
        mag_data[2] = (((int16_t)raw_data[5]) << 8) | raw_data[4];
    }
}

void MPU9250_accelerometer(float *accel)
{
    int16_t raw_data[3];
    MPU9250_raw_accelerometer(raw_data);

    accel[0] = ((float)raw_data[0])*accel_sca;
    accel[1] = ((float)raw_data[1])*accel_sca;
    accel[2] = ((float)raw_data[2])*accel_sca;
}

void MPU9250_gyroscope(float *gyro)
{
    int16_t raw_data[3];
    MPU9250_raw_gyroscope(raw_data);

    gyro[0] = ((float)raw_data[0]*gyro_sca);
    gyro[1] = ((float)raw_data[1]*gyro_sca);
    gyro[2] = ((float)raw_data[2]*gyro_sca);

    //Convert to rads/s
    gyro[0] *= M_PI/180.0f;
    gyro[1] *= M_PI/180.0f;
    gyro[2] *= M_PI/180.0f;
}

void AK8963_magnetometer(float *mag)
{
    int16_t raw_data[3];
    AK8963_raw_magnetometer(raw_data);

    mag[0] = raw_data[0]*mag_scale;
    mag[1] = raw_data[1]*mag_scale;
    mag[2] = raw_data[2]*mag_scale;
}

void MPU9250_accelerometer_range(uint8_t range)
{
    switch(range)
    {
    case AFS_2G:
        MPU9250_write_byte(ACCEL_CONFIG, AFS_2G << 3);
        accel_sca = 2.0f/32768.0f;
        accel_res = AFS_2G;
        break;
    case AFS_4G:
        MPU9250_write_byte(ACCEL_CONFIG, AFS_4G << 3);
        accel_sca = 4.0f/32768.0f;
        accel_res = AFS_4G;
        break;
    case AFS_8G:
        MPU9250_write_byte(ACCEL_CONFIG, AFS_8G << 3);
        accel_sca = 8.0f/32768.0f;
        accel_res = AFS_8G;
        break;
    case AFS_16G:
        MPU9250_write_byte(ACCEL_CONFIG, AFS_16G << 3);
        accel_sca = 16.0f/32768.0f;
        accel_res = AFS_16G;
        break;
    }
}

void MPU9250_gyroscope_range(uint8_t range)
{
    switch(range)
    {
    case GFS_250DPS:
        MPU9250_write_byte(GYRO_CONFIG, GFS_250DPS << 3);
        gyro_sca = 250.0f/32768.0f;
        gyro_res = GFS_250DPS;
        break;
    case GFS_500DPS:
        MPU9250_write_byte(GYRO_CONFIG, GFS_500DPS << 3);
        gyro_sca = 500.0f/32768.0f;
        gyro_res = GFS_500DPS;
        break;
    case GFS_1000DPS:
        MPU9250_write_byte(GYRO_CONFIG, GFS_1000DPS << 3);
        gyro_sca = 1000.0f/32768.0f;
        gyro_res = GFS_1000DPS;
        break;
    case GFS_2000DPS:
        MPU9250_write_byte(GYRO_CONFIG, GFS_2000DPS << 3);
        gyro_sca = 2500.0f/32768.0f;
        gyro_res = GFS_2000DPS;
        break;
    }
}

void MPU9250_calibrate_gyroscope(void)
{

}

void MPU9250_calibrate_accelerometer(void)
{

}

void AK8963_calibrate_magnetometer(void)
{

}


void AK8963_write_byte(uint8_t reg, uint8_t value)
{
    uint8_t cmd[] = {reg, value};
    i2c_dev_write(AK8963_ADDRESS, cmd, 2);
}

void MPU9250_write_byte(uint8_t reg, uint8_t value)
{
    uint8_t cmd[] = {reg, value};
    i2c_dev_write(MPU9250_1_ADDRESS, cmd, 2);
}

