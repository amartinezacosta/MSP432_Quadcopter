#include "MPU6050.h"
#include "QMC5883.h"
#include "ESC.h"
#include "UARTDEBUG.h"

#include "EasyHal/time_dev.h"

#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

#define TELEMETRY_QUEUE_SIZE    256

typedef struct
{
    int16_t raw_accel[3];
    int16_t raw_gyro[3];
    int16_t raw_mag[3];
    float dt;
    float time;
}telemetry_t;

void *telemetry_thread(void *arg0);

void *mainThread(void *arg0)
{
    time_dev_init();

    UARTDEBUG_init(9600);
    MPU6050_init(MPU6050_DEFAULT_ADDRESS);
    QMC5883_init();
    ESC_init();

    ESC_arm();
    ESC_calibrate();

    telemetry_t packet;
    int16_t raw_accel[3];
    int16_t raw_gyro[3];
    int16_t raw_mag[3];

    uint32_t start;
    float dt = 0.0;
    float t = 0.0;

    //Setup queue
    mqd_t mq;
    struct mq_attr attr;

    //Setup queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = TELEMETRY_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(telemetry_t);
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

    pthread_create(&tid, NULL, telemetry_thread, mq);

    //Wait here until we can start drone
    char c;
    UARTDEBUG_gets(&c, 1, false);

    while(1)
    {
        start = millis();

        MPU6050_raw_accelerometer(raw_accel);
        MPU6050_raw_gyroscope(raw_gyro);
        QMC5883_raw_magnetometer(raw_mag);

        //Create telemetry packet
        uint32_t i;
        for(i = 0; i < 3; i++)
        {
            packet.raw_accel[i] = raw_accel[i];
            packet.raw_gyro[i] = raw_gyro[i];
            packet.raw_mag[i] = raw_mag[i];
        }

        packet.time = dt;

        //Queue telemetry packet
        mq_send(mq, (char*)&packet, sizeof(telemetry_t), NULL);

        usleep(4000);

        dt = (millis() - start)/1e3;
        t += dt;
    }
}

void *telemetry_thread(void *arg0)
{
    mqd_t mq = (mqd_t)arg0;
    telemetry_t packet;
    int status;

    while(1)
    {
        status = mq_receive(mq, (char*)&packet, sizeof(telemetry_t), NULL);

        if(status > 0)
        {
            UARTDEBUG_printf("%f, %i, %i, %i, %i, %i, %i, %i, %i, %i\n", packet.time, packet.raw_accel[0], packet.raw_accel[1], packet.raw_accel[2],
                                                                                      packet.raw_gyro[0], packet.raw_gyro[1], packet.raw_gyro[2],
                                                                                      packet.raw_mag[0], packet.raw_mag[1], packet.raw_mag[2]);
        }

    }
}

