#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <stdint.h>

#define TELEMETRY_BUFFER_SIZE   512

/*
#define TELEMETRY_QUEUE_SIZE    10

typedef struct
{
    //Orientation data
    float angle[3];
    //IMU data
    float gyro[3];
    float accel[3];
    float mag[3];
    //GPS data
    float location[3];
    uint32_t satellites;
    //Pressure data
    float pressure;
    float altitude;
    //Time data
    float dt;
    float time;
}tel_packet_t;
*/

void telemetry_init(void);
//void telemetry_queue(float *angle, float *gyro, float *accel, float *mag, float *location, uint32_t satellites, float pressure, float altitude, float dt, float t);
void telemetry_send(float *angle, float *gyro, float *accel, float *mag, float *location, uint32_t satellites, float pressure, float altitude, float dt, float t);

#endif /* TELEMETRY_H_ */
