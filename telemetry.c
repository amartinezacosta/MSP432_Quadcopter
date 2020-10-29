#include "telemetry.h"
#include "UARTDEBUG.h"

#include <pthread.h>
#include <mqueue.h>

void *telemetry_thread(void *arg0);

mqd_t mq;

void telemetry_init(void)
{
    //Setup queue
    struct mq_attr attr;

    //Setup queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = TELEMETRY_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(tel_packet_t);
    attr.mq_curmsgs = 0;

    //Create queue
    mq = mq_open("TELEMETRY_QUEUE", O_CREAT | O_NONBLOCK | O_RDWR, 0644, &attr);

    //Create telemetry thread
    pthread_t tid;
    pthread_attr_t      attrs;
    struct sched_param  priParam;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&attrs, &priParam);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attrs, 256);

    pthread_create(&tid, NULL, telemetry_thread, NULL);
}

void telemetry_queue(float *angle, float *gyro, float *accel, float *mag, float *location, uint32_t satellites, float pressure, float altitude, float dt, float t)
{
    //Create telemetry packet
    tel_packet_t packet;

    uint32_t i;
    for(i = 0; i < 3; i++)
    {
        packet.angle[i] = angle[i];
        packet.gyro[i] = gyro[i];
        packet.accel[i] = accel[i];
        packet.mag[i] = mag[i];
        packet.location[i] = location[i];
    }

    packet.satellites = satellites;
    packet.pressure = pressure;
    packet.altitude = altitude;
    packet.dt = dt;
    packet.time = t;

    //Queue telemetry packet
    mq_send(mq, (char*)&packet, sizeof(tel_packet_t), NULL);
}

/*Telemetry packet protocol
 *
 * Every telemetry packet starts with a 'TEL:' heading and it is sent using ASCII characters and is terminated with a new line character '\n'
 *
 * TEL:pitch,roll,yaw,gyro_x,gyro_y,gyro_z,accel_x,accel_y,accel_z,gps_lat,gps_long,gps_elevation,gps_satellites,pressure,altitude,dt,time
 *
 * */
void *telemetry_thread(void *arg0)
{
    tel_packet_t packet;
    int status;

    while(1)
    {
        status = mq_receive(mq, (char*)&packet, sizeof(tel_packet_t), NULL);
        if(status > 0)
        {
            UARTDEBUG_printf("TEL:%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%i,%f,%f,%f,%f\n", packet.angle[0], packet.angle[1], packet.angle[2],
                                                                                         packet.gyro[0], packet.gyro[1], packet.gyro[2],
                                                                                         packet.accel[0], packet.accel[2], packet.accel[2],
                                                                                         packet.location[0], packet.location[1], packet.location[2],
                                                                                         packet.satellites, packet.pressure, packet.altitude,
                                                                                         packet.dt, packet.time);
        }

    }
}
