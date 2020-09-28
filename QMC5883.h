#ifndef QMC5883_H_
#define QMC5883_H_

#include <stdint.h>

#define HMC5883L_ADDRESS                0x1E
#define QMC5883_ADDRESS                 0x0D

#define HMC5883L_REG_CONFIG_A           0x00
#define HMC5883L_REG_CONFIG_B           0x01
#define HMC5883L_REG_MODE               0x02
#define HMC5883L_REG_OUT_X_M            0x03
#define HMC5883L_REG_OUT_X_L            0x04
#define HMC5883L_REG_OUT_Z_M            0x05
#define HMC5883L_REG_OUT_Z_L            0x06
#define HMC5883L_REG_OUT_Y_M            0x07
#define HMC5883L_REG_OUT_Y_L            0x08
#define HMC5883L_REG_STATUS             0x09
#define HMC5883L_REG_IDENT_A            0x0A
#define HMC5883L_REG_IDENT_B            0x0B
#define HMC5883L_REG_IDENT_C            0x0C

#define QMC5883_REG_OUT_X_L             0x00
#define QMC5883_REG_OUT_X_M             0x01
#define QMC5883_REG_OUT_Y_L             0x02
#define QMC5883_REG_OUT_Y_M             0x03
#define QMC5883_REG_OUT_Z_L             0x04
#define QMC5883_REG_OUT_Z_M             0x05

#define QMC5883_REG_STATUS              0x06
#define QMC5883_REG_CONFIG_1            0x09
#define QMC5883_REG_CONFIG_2            0x0A
#define QMC5883_REG_IDENT_B             0x0B
#define QMC5883_REG_IDENT_C             0x20
#define QMC5883_REG_IDENT_D             0x21

#define HMC5883L_SAMPLES_8              0x03
#define HMC5883L_SAMPLES_4              0x02
#define HMC5883L_SAMPLES_2              0x01
#define HMC5883L_SAMPLES_1              0x00

#define QMC5883_SAMPLES_8               0x03
#define QMC5883_SAMPLES_4               0x02
#define QMC5883_SAMPLES_2               0x01
#define QMC5883_SAMPLES_1               0x00

#define HMC5883L_DATARATE_75HZ         0x06
#define HMC5883L_DATARATE_30HZ         0x05
#define HMC5883L_DATARATE_15HZ         0x04
#define HMC5883L_DATARATE_7_5HZ        0x03
#define HMC5883L_DATARATE_3HZ          0x02
#define HMC5883L_DATARATE_1_5HZ        0x01
#define HMC5883L_DATARATE_0_75_HZ      0x00

#define QMC5883_DATARATE_10HZ          0x00
#define QMC5883_DATARATE_50HZ          0x01
#define QMC5883_DATARATE_100HZ         0x02
#define QMC5883_DATARATE_200HZ         0x03

#define HMC5883L_RANGE_8_1GA            0x07
#define HMC5883L_RANGE_5_6GA            0x06
#define HMC5883L_RANGE_4_7GA            0x05
#define HMC5883L_RANGE_4GA              0x04
#define HMC5883L_RANGE_2_5GA            0x03
#define HMC5883L_RANGE_1_9GA            0x02
#define HMC5883L_RANGE_1_3GA            0x01
#define HMC5883L_RANGE_0_88GA           0x00

#define QMC5883_RANGE_2GA               0x00
#define QMC5883_RANGE_8GA               0x01

#define HMC5883L_IDLE                   0x02
#define HMC5883_SINGLE                  0x01
#define HMC5883L_CONTINOUS              0x00

#define QMC5883_SINGLE                  0x00
#define QMC5883_CONTINOUS               0x01

void QMC5883_init(void);
void QMC5883_set_samples(uint8_t samples);
void QMC5883_set_rate(uint8_t rate);
void QMC5883_set_measurement(uint8_t mode);
void QMC5883_set_range(uint8_t range);
void QMC5883_raw_magnetometer(int16_t *mag);

#endif /* QMC5883_H_ */
