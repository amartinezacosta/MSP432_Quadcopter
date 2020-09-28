#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include <stdbool.h>

#define BME280_ADDRESS              0x77
#define BME280_ADDRESS_ALTERNATE    0x76

#define BME280_REGISTER_DIG_T1      0x88
#define BME280_REGISTER_DIG_T2      0x8A
#define BME280_REGISTER_DIG_T3      0x8C

#define BME280_REGISTER_DIG_P1      0x8E
#define BME280_REGISTER_DIG_P2      0x90
#define BME280_REGISTER_DIG_P3      0x92
#define BME280_REGISTER_DIG_P4      0x94
#define BME280_REGISTER_DIG_P5      0x96
#define BME280_REGISTER_DIG_P6      0x98
#define BME280_REGISTER_DIG_P7      0x9A
#define BME280_REGISTER_DIG_P8      0x9C
#define BME280_REGISTER_DIG_P9      0x9E

#define BME280_REGISTER_DIG_H1      0xA1
#define BME280_REGISTER_DIG_H2      0xE1
#define BME280_REGISTER_DIG_H3      0xE3
#define BME280_REGISTER_DIG_H4      0xE4
#define BME280_REGISTER_DIG_H5      0xE5
#define BME280_REGISTER_DIG_H6      0xE7

#define BME280_REGISTER_CHIPID          0xD0
#define BME280_REGISTER_VERSION         0xD1
#define BME280_REGISTER_SOFTRESET       0xE0

#define BME280_REGISTER_CAL26           0xE1 // R calibration stored in 0xE1-0xF0

#define BME280_REGISTER_CONTROLHUMID    0xF2
#define BME280_REGISTER_STATUS          0XF3
#define BME280_REGISTER_CONTROL         0xF4
#define BME280_REGISTER_CONFIG          0xF5
#define BME280_REGISTER_PRESSUREDATA    0xF7
#define BME280_REGISTER_TEMPDATA        0xFA
#define BME280_REGISTER_HUMIDDATA       0xFD

#define MODE_SLEEP          0x00
#define MODE_FORCED         0x01
#define MODE_NORMAL         0x03

#define STANDBY_MS_0_5      0x00
#define STANDBY_MS_10       0x06
#define STANDBY_MS_20       0x07
#define STANDBY_MS_62_5     0x01
#define STANDBY_MS_125      0x02
#define STANDBY_MS_250      0x03
#define STANDBY_MS_500      0x04
#define STANDBY_MS_1000     0x05

#define FILTER_OFF          0x00
#define FILTER_X2           0x01
#define FILTER_X4           0x02
#define FILTER_X8           0x03
#define FILTER_X16          0x04

#define SAMPLING_NONE       0x00
#define SAMPLING_X1         0x01
#define SAMPLING_X2         0x02
#define SAMPLING_X4         0x03
#define SAMPLING_X8         0x04
#define SAMPLING_X16        0x05

void BME280_init(void);
bool BME280_calibrating(void);
void BME280_mode(uint8_t mode);
uint8_t BME280_get_mode(void);
void BME280_standby(uint8_t time);
void BME280_filter(uint8_t filter);
void BME280_temperature_sampling(uint8_t sampling);
void BME280_pressure_sampling(uint8_t sampling);
void BME280_humid_sampling(uint8_t sampling);
void BME280_coefficients(void);
float BME280_pressure(void);
float BME280_temperature(void);
float BME280_humidity(void);
float BME280_altitude(float sea_level);
uint8_t BME280_who(void);

//TODO: Move to i2c_dev?

#endif /* BME280_H_ */
