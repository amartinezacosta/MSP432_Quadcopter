#include "IMU.h"
#include <math.h>

float compass_heading(float mx, float my, float mz, float *pitch, float *roll)
{
    float pitch_rad = -*pitch*0.0174533;
    float roll_rad = -*roll*0.0174533;

    float hx = mx*cos(pitch_rad) + my*sin(roll_rad)*sin(pitch_rad) - mz*cos(roll_rad)*sin(pitch_rad);
    float hy = my*cos(roll_rad) + mz*sin(roll_rad);

    float heading = atan2(hy, hx)*57.296;

    if(hy < 0 ) heading = 180 + (180 + atan2(hy, hx)*57.296);
    else        heading = atan2(hy, hx)*57.296;


    return heading;
}

void accelerometer_angles(float ax, float ay, float az, float *pitch, float *roll)
{
    float accel_magnitude = sqrt(ax*ax + ay*ay + az*az);
    *pitch = asin(ay/accel_magnitude)*57.296;
    *roll = -asin(ax/accel_magnitude)*57.296;
}

void gyroscope_angles(float gx, float gy, float gz, float *pitch, float *roll, float *yaw, float dt)
{
    *pitch += gx*dt;
    *roll += gy*dt;
    *yaw += gz*dt;

    *pitch -= *roll*sin(gz*0.0174533*dt);
    *roll +=  *pitch*sin(gz*0.0174533*dt);
}
