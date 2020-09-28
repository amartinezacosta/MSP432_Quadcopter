#include "QMC5883.h"
#include "EasyHal/i2c_dev.h"

void QMC5883_write_byte(uint8_t reg, uint8_t value);
uint8_t QMC5883_read_byte(uint8_t reg);

void QMC5883_init(void)
{
    i2c_dev_init(I2C_DEV_400KHZ);

    QMC5883_write_byte(QMC5883_REG_IDENT_B, 0x01);
    QMC5883_write_byte(QMC5883_REG_IDENT_C, 0x40);
    QMC5883_write_byte(QMC5883_REG_IDENT_D, 0x01);
    QMC5883_write_byte(QMC5883_REG_CONFIG_1, 0x1D);

    QMC5883_set_range(QMC5883_RANGE_8GA);
    QMC5883_set_measurement(QMC5883_CONTINOUS);
    QMC5883_set_rate(QMC5883_DATARATE_50HZ);
    QMC5883_set_samples(QMC5883_SAMPLES_8);
}

void QMC5883_set_samples(uint8_t samples)
{
    uint8_t value = QMC5883_read_byte(QMC5883_REG_CONFIG_1);
    value &= 0x3F;
    value |= (samples << 6);

    QMC5883_write_byte(QMC5883_REG_CONFIG_1, value);
}

void QMC5883_set_rate(uint8_t rate)
{
    uint8_t value = QMC5883_read_byte(QMC5883_REG_CONFIG_1);
    value &= 0xF3;
    value |= (rate << 2);

    QMC5883_write_byte(QMC5883_REG_CONFIG_1, value);
}

void QMC5883_set_measurement(uint8_t mode)
{
    uint8_t value = QMC5883_read_byte(QMC5883_REG_CONFIG_1);
    value &= 0xFC;
    value |= mode;

    QMC5883_write_byte(QMC5883_REG_CONFIG_1, value);
}

void QMC5883_set_range(uint8_t range)
{
    QMC5883_write_byte(QMC5883_REG_CONFIG_2, range << 4);
}

void QMC5883_raw_magnetometer(int16_t *mag)
{
    uint8_t raw_data[6];
    i2c_dev_read(QMC5883_ADDRESS, QMC5883_REG_OUT_X_L, raw_data, 6);

    mag[0] = ((int16_t)(raw_data[1] << 8)) | raw_data[0];
    mag[1] = ((int16_t)(raw_data[3] << 8)) | raw_data[2];
    mag[2] = ((int16_t)(raw_data[5] << 8)) | raw_data[4];
}

/*LOW LEVEL QMC5883 COMMUNICATION-----------------------------------------------------*/

void QMC5883_write_byte(uint8_t reg, uint8_t value)
{
    uint8_t cmd[] = {reg, value};
    i2c_dev_write(QMC5883_ADDRESS, cmd, 2);
}

uint8_t QMC5883_read_byte(uint8_t reg)
{
    uint8_t data;
    i2c_dev_read(QMC5883_ADDRESS, reg, &data, 1);

    return data;
}

/*LOW LEVEL QMC5883 COMMUNICATION-----------------------------------------------------*/
