#ifndef IMU_H_
#define IMU_H_

float compass_heading(float mx, float my, float mz, float *pitch, float *roll);
void accelerometer_angles(float ax, float ay, float az, float *pitch, float *roll);
void gyroscope_angles(float gx, float gy, float gz, float *pitch, float *roll, float *yaw, float dt);

#endif /* IMU_H_ */
