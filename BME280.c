#include "BME280.h"
#include <math.h>
#include "EasyHal/i2c_dev.h"
#include "EasyHal/time_dev.h"

//Based on Adafruit and Sparkfun libraries

static uint16_t dig_T1; //temperature compensation value
static int16_t  dig_T2;  //temperature compensation value
static int16_t  dig_T3;  //temperature compensation value

static uint16_t dig_P1; //pressure compensation value
static int16_t  dig_P2;  //pressure compensation value
static int16_t  dig_P3;  //pressure compensation value
static int16_t  dig_P4;  //pressure compensation value
static int16_t  dig_P5;  //pressure compensation value
static int16_t  dig_P6;  //pressure compensation value
static int16_t  dig_P7;  //pressure compensation value
static int16_t  dig_P8;  //pressure compensation value
static int16_t  dig_P9;  //pressure compensation value

static uint8_t dig_H1; //humidity compensation value
static int16_t dig_H2; //humidity compensation value
static uint8_t dig_H3; //humidity compensation value
static int16_t dig_H4; //humidity compensation value
static int16_t dig_H5; //humidity compensation value
static int8_t  dig_H6;  //humidity compensation value

static int32_t t_fine_adjust = 0; //add to compensate temp readings and in turn to pressure and humidity readings
static int32_t t_fine;            //temperature with high resolution, stored as an attribute as this is used for temperature compensation reading humidity and pressure

void BME280_write_uint8(uint8_t reg, uint8_t value);
uint8_t BME280_read_uint8(uint8_t reg);
int16_t BME280_read_int16(uint8_t reg);
uint16_t BME280_read_uint16(uint8_t reg);

void BME280_init(void)
{
    i2c_dev_init(I2C_DEV_400KHZ);

    //reset device
    BME280_write_uint8(BME280_REGISTER_SOFTRESET, 0xB6);
    delay(10);

    //Wait for calibration to finish
    while(BME280_calibrating())
    {
        delay(10);
    }

    BME280_coefficients();
    BME280_standby(STANDBY_MS_0_5);
    BME280_filter(FILTER_OFF);
    BME280_temperature_sampling(SAMPLING_X1);
    BME280_pressure_sampling(SAMPLING_X1);
    BME280_humid_sampling(SAMPLING_X1);

    BME280_mode(MODE_NORMAL);

    delay(100);
}

bool BME280_calibrating(void)
{
    uint8_t status = BME280_read_uint8(BME280_REGISTER_STATUS);

    return (status & 0x01);
}

void BME280_coefficients(void)
{
    dig_T1 = BME280_read_uint16(BME280_REGISTER_DIG_T1);
    dig_T2 = BME280_read_int16(BME280_REGISTER_DIG_T2);
    dig_T3 = BME280_read_int16(BME280_REGISTER_DIG_T3);

    dig_P1 = BME280_read_uint16(BME280_REGISTER_DIG_P1);
    dig_P2 = BME280_read_int16(BME280_REGISTER_DIG_P2);
    dig_P3 = BME280_read_int16(BME280_REGISTER_DIG_P3);
    dig_P4 = BME280_read_int16(BME280_REGISTER_DIG_P4);
    dig_P5 = BME280_read_int16(BME280_REGISTER_DIG_P5);
    dig_P6 = BME280_read_int16(BME280_REGISTER_DIG_P6);
    dig_P7 = BME280_read_int16(BME280_REGISTER_DIG_P7);
    dig_P8 = BME280_read_int16(BME280_REGISTER_DIG_P8);
    dig_P9 = BME280_read_int16(BME280_REGISTER_DIG_P9);

    dig_H1 = BME280_read_uint8(BME280_REGISTER_DIG_H1);
    dig_H2 = BME280_read_int16(BME280_REGISTER_DIG_H2);
    dig_H3 = BME280_read_uint8(BME280_REGISTER_DIG_H3);
    dig_H4 = ((int8_t)BME280_read_uint8(BME280_REGISTER_DIG_H4) << 4)     | (BME280_read_uint8(BME280_REGISTER_DIG_H4 + 1) & 0xF);
    dig_H5 = ((int8_t)BME280_read_uint8(BME280_REGISTER_DIG_H5 + 1) << 4) | (BME280_read_uint8(BME280_REGISTER_DIG_H5) >> 4);
    dig_H6 = (int8_t)BME280_read_uint8(BME280_REGISTER_DIG_H6);
}

void BME280_mode(uint8_t mode)
{
    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONTROL);

    control &= ~0x03;
    control |= mode;
    BME280_write_uint8(BME280_REGISTER_CONTROL, control);
}

uint8_t BME280_get_mode(void)
{
    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONTROL);
    return (control & 0x03);
}

void BME280_standby(uint8_t time)
{
    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONFIG);

    control &= ~0xD0;
    control |= (time << 5);
    BME280_write_uint8(BME280_REGISTER_CONFIG, control);
}

void BME280_filter(uint8_t filter)
{
    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONFIG);

    control &= ~0x0D;
    control |= (filter << 5);
    BME280_write_uint8(BME280_REGISTER_CONFIG, control);
}

void BME280_temperature_sampling(uint8_t sampling)
{
    uint8_t mode = BME280_get_mode();
    BME280_mode(MODE_SLEEP);

    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONTROL);
    control &= ~0xD0;
    control |= (sampling << 5);
    BME280_write_uint8(BME280_REGISTER_CONTROL, control);

    BME280_mode(mode);
}


void BME280_pressure_sampling(uint8_t sampling)
{
    uint8_t mode = BME280_get_mode();
    BME280_mode(MODE_SLEEP);

    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONTROL);
    control &= ~0x0D;
    control |= (sampling << 2);
    BME280_write_uint8(BME280_REGISTER_CONTROL, control);

    BME280_mode(mode);
}


void BME280_humid_sampling(uint8_t sampling)
{
    uint8_t mode = BME280_get_mode();
    BME280_mode(MODE_SLEEP);

    uint8_t control = BME280_read_uint8(BME280_REGISTER_CONTROLHUMID);
    control &= ~0x07;
    control |= (sampling << 0);
    BME280_write_uint8(BME280_REGISTER_CONTROLHUMID, control);

    BME280_mode(mode);
}

float BME280_temperature(void)
{
    uint8_t raw_data[3];
    int32_t var1, var2;

    i2c_dev_read(BME280_ADDRESS_ALTERNATE, BME280_REGISTER_TEMPDATA, raw_data, 3);
    int32_t adc_T = (int32_t)(((int32_t)(raw_data[0] << 16)) | ((int32_t)(raw_data[1] << 8)) | raw_data[2]);

    if(adc_T == 0x800000) return NAN;
    adc_T >>= 4;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >>11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2 + t_fine_adjust;

    float T = (t_fine * 5 + 128) >> 8;
    return T / 100;
}

float BME280_pressure(void)
{
    uint8_t raw_data[3];
    int64_t var1, var2, p;

    //Must be done to get t_fine
    BME280_temperature();

    i2c_dev_read(BME280_ADDRESS_ALTERNATE, BME280_REGISTER_PRESSUREDATA, raw_data, 3);
    int32_t adc_P = (int32_t)(((int32_t)(raw_data[0] << 16)) | ((int32_t)(raw_data[1] << 8)) | raw_data[2]);

    if(adc_P == 0x800000) return NAN;
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

    if (var1 == 0) return 0; // avoid exception caused by division by zero

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (float)p / 256.0f;
}

float BME280_humidity(void)
{
    uint8_t raw_data[2];

    //Must be done to get t_fine
    BME280_temperature();

    i2c_dev_read(BME280_ADDRESS_ALTERNATE, BME280_REGISTER_HUMIDDATA, raw_data, 2);
    int32_t adc_H = (int32_t)((uint16_t)(raw_data[0] << 8) | raw_data[1]);

    if(adc_H == 0x8000) return NAN;

    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) -
                (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
                ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                ((int32_t)dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) *
                (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float h = (v_x1_u32r >> 12);
    return h / 1024.0;
}

float BME280_altitude(float sea_level)
{
    float atmospheric = BME280_pressure()/100.0f;
    return 44330.0f * (1.0f - pow(atmospheric/sea_level, 0.1903));
}

uint8_t BME280_who(void)
{
    return BME280_read_uint8(BME280_REGISTER_CHIPID);
}

void BME280_write_uint8(uint8_t reg, uint8_t value)
{
    uint8_t cmd[] = {reg, value};
    i2c_dev_write(BME280_ADDRESS_ALTERNATE, cmd, 2);
}

uint8_t BME280_read_uint8(uint8_t reg)
{
    uint8_t raw_data;
    i2c_dev_read(BME280_ADDRESS_ALTERNATE, reg, &raw_data, 1);

    return raw_data;
}

int16_t BME280_read_int16(uint8_t reg)
{
    uint8_t raw_data[2];
    i2c_dev_read(BME280_ADDRESS_ALTERNATE, reg, raw_data, 2);

    int16_t value = ((uint16_t)(raw_data[1] << 8) | raw_data[0]);

    return value;
}

uint16_t BME280_read_uint16(uint8_t reg)
{
    uint8_t raw_data[2];
    i2c_dev_read(BME280_ADDRESS_ALTERNATE, reg, raw_data, 2);

    uint16_t value = ((uint16_t)(raw_data[1] << 8) | raw_data[0]);

    return value;
}

