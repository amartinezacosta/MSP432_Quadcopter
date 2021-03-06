#include "MPU6050.h"
#include "EasyHal/i2c_dev.h"
#include "EasyHal/time_dev.h"
#include "LED.h"

//Based on jrowberg I2Cdev library

uint8_t MPU6050_address = MPU6050_DEFAULT_ADDRESS;

void MPU6050_write_byte(uint8_t reg, uint8_t value);
void MPU6050_write_bit(uint8_t reg, uint8_t n, uint8_t value);
void MPU6050_write_bits(uint8_t reg, uint8_t start, uint8_t size, uint8_t value);

uint8_t MPU6050_read_byte(uint8_t reg);
uint8_t MPU6050_read_bit(uint8_t reg, uint8_t n);
uint8_t MPU6050_read_bits(uint8_t reg, uint8_t start, uint8_t size);

void MPU6050_init(uint8_t address)
{
    i2c_dev_init(I2C_DEV_400KHZ);
    MPU6050_address = address;

    MPU6050_set_clock(MPU6050_CLOCK_PLL_XGYRO);
    MPU6050_gyroscope_range(MPU6050_GYRO_FS_500);
    MPU6050_accelerometer_range(MPU6050_ACCEL_FS_8);
    MPU6050_sleep(false);

    delay(10);
}

void MPU6050_set_clock(uint8_t source)
{
    MPU6050_write_bits(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

void MPU6050_set_rate(uint8_t rate)
{
    MPU6050_write_byte(MPU6050_RA_SMPLRT_DIV, rate);
}

void MPU6050_set_DLPF(uint8_t mode)
{
    MPU6050_write_bits(MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}

void MPU6050_gyroscope_range(uint8_t range)
{
    MPU6050_write_bits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

void MPU6050_accelerometer_range(uint8_t range)
{
    MPU6050_write_bits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

void MPU6050_set_DHPF(uint8_t bandwidth)
{
    MPU6050_write_bits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ACCEL_HPF_BIT, MPU6050_ACONFIG_ACCEL_HPF_LENGTH, bandwidth);
}

void MPU6050_sleep(bool sleep)
{
    MPU6050_write_bit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, sleep);
}

void MPU6050_reset(void)
{
    MPU6050_write_bit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET_BIT, true);
    delay(50);
}

uint8_t MPU6050_who(void)
{
    return MPU6050_read_byte(MPU6050_RA_WHO_AM_I);
}

void MPU6050_raw_temperature(int16_t *temp)
{
    uint8_t raw_data[2];
    i2c_dev_read(MPU6050_address, MPU6050_RA_TEMP_OUT_H, raw_data, 2);

    *temp = (((int16_t)raw_data[0]) << 8) | raw_data[1];
}

void MPU6050_raw_accelerometer(int16_t *accel)
{
    uint8_t raw_data[6];
    i2c_dev_read(MPU6050_address, MPU6050_RA_ACCEL_XOUT_H, raw_data, 6);

    accel[0] = ((int16_t)raw_data[0] << 8) | raw_data[1];
    accel[1] = ((int16_t)raw_data[2] << 8) | raw_data[3];
    accel[2] = ((int16_t)raw_data[4] << 8) | raw_data[5];
}

void MPU6050_raw_gyroscope(int16_t *gyro)
{
    uint8_t raw_data[6];
    i2c_dev_read(MPU6050_address, MPU6050_RA_GYRO_XOUT_H, raw_data, 6);

    gyro[0] = ((int16_t)raw_data[0] << 8) | raw_data[1];
    gyro[1] = ((int16_t)raw_data[2] << 8) | raw_data[3];
    gyro[2] = ((int16_t)raw_data[4] << 8) | raw_data[5];
}

void MPU6050_calibrate_accelerometer(int32_t *offsets, uint32_t dt)
{
    int16_t raw_accel[3];
    uint8_t led_value = 0;

    //Calibrate accelerometer
    uint32_t i;
    for(i = 0; i < MPU6050_ACCEL_CALIBRATION; i++)
    {
        MPU6050_raw_accelerometer(raw_accel);

        offsets[0] += raw_accel[0];
        offsets[1] += raw_accel[1];
        raw_accel[2] -= 4096;

        offsets[2] += raw_accel[2];

        if((i % 32) == 0)
        {
            led_value = (led_value == 0) ? 1 : 0;
            LED_solid(RED_LED, led_value);
        }

        delay(dt);
    }

    offsets[0] /= 200;
    offsets[1] /= 200;
    offsets[2] /= 200;

    LED_solid(RED_LED, 1);
}

void MPU6050_calibrate_gyroscope(int32_t *offsets, uint32_t dt)
{
    int16_t raw_gyro[3];
    uint8_t led_value = 0;

    //Calibrate gyroscope
    uint32_t i;
    for(i = 0; i < MPU6050_GYRO_CALIBRATION; i++)
    {
        MPU6050_raw_gyroscope(raw_gyro);

        offsets[0] += raw_gyro[0];
        offsets[1] += raw_gyro[1];
        offsets[2] += raw_gyro[2];

        if((i % 16) == 0)
        {
            led_value = (led_value == 0) ? 1 : 0;
            LED_solid(RED_LED, led_value);
        }

        delay(dt);
    }

    offsets[0] /= 2000;
    offsets[1] /= 2000;
    offsets[2] /= 2000;

    LED_solid(RED_LED, 1);
}

void MPU6050_gyroscope(float *gyrosocope, int32_t *offsets)
{
    int16_t raw_gyro[3];
    MPU6050_raw_gyroscope(raw_gyro);

    raw_gyro[0] -= offsets[0];
    raw_gyro[1] -= offsets[1];
    raw_gyro[2] -= offsets[2];

    //Assuming +-250dps
    gyrosocope[0] = ((float)raw_gyro[0])*0.01526;
    gyrosocope[1] = ((float)raw_gyro[1])*0.01526;
    gyrosocope[2] = ((float)raw_gyro[2])*0.01526;
}

void MPU6050_accelerometer(float *accelerometer, int32_t *offsets)
{
    int16_t raw_accel[3];
    MPU6050_raw_accelerometer(raw_accel);

    raw_accel[0] -= offsets[0];
    raw_accel[1] -= offsets[1];
    raw_accel[2] -= offsets[2];

    //Accelerometer reading, assuming +-2g
    accelerometer[0] = ((float)raw_accel[0])*0.00024414;
    accelerometer[1] = ((float)raw_accel[1])*0.00024414;
    accelerometer[2] = ((float)raw_accel[2])*0.00024414;
}

/*LOW LEVEL MPU6050 COMMUNICATION--------------------------------------------------*/
void MPU6050_write_byte(uint8_t reg, uint8_t value)
{
    uint8_t cmd[] = {reg, value};
    i2c_dev_write(MPU6050_address, cmd, 2);
}

void MPU6050_write_bit(uint8_t reg, uint8_t n, uint8_t value)
{
    uint8_t data;
    uint8_t cmd[2];

    i2c_dev_read(MPU6050_address, reg, &data, 1);

    data = (value != 0) ? (data | (1 << n)) : (data & ~(1 << n));

    cmd[0] = reg;
    cmd[1] = data;
    i2c_dev_write(MPU6050_address, cmd, 2);
}

void MPU6050_write_bits(uint8_t reg, uint8_t start, uint8_t size, uint8_t value)
{
    uint8_t data;
    uint8_t cmd[2];

    i2c_dev_read(MPU6050_address, reg, &data, 1);

    uint8_t mask = ((1 << size) - 1) << (start - size + 1);
    value <<= (start - size + 1);
    value &= mask;
    data &= ~mask;
    data |= value;

    cmd[0] = reg;
    cmd[1] = data;
    i2c_dev_write(MPU6050_address, cmd, 2);
}

uint8_t MPU6050_read_byte(uint8_t reg)
{
    uint8_t data;
    i2c_dev_read(MPU6050_address, reg, &data, 1);

    return data;
}

uint8_t MPU6050_read_bit(uint8_t reg, uint8_t n)
{
    uint8_t data;
    i2c_dev_read(MPU6050_address, reg, &data, 1);

    return (data & (1 << n));
}

uint8_t MPU6050_read_bits(uint8_t reg, uint8_t start, uint8_t size)
{
    uint8_t data;
    i2c_dev_read(MPU6050_address, reg, &data, 1);

    uint8_t mask = ((1 << size) - 1) << (start - size + 1);
    data &= mask;
    data >>= (start - size + 1);

    return data;
}
/*LOW LEVEL MPU6050 COMMUNICATION--------------------------------------------------*/
