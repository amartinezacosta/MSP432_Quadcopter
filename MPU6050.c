#include "MPU6050.h"
#include "EasyHal/i2c_dev.h"
#include "EasyHal/time_dev.h"

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
    MPU6050_gyroscope_range(MPU6050_GYRO_FS_250);
    MPU6050_accelerometer_range(MPU6050_ACCEL_FS_2);
    MPU6050_sleep(false);
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
