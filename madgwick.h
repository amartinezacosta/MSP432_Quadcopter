#ifndef MADGWICK_H_
#define MADGWICK_H_

void Madgwick_init(void);
void Madgwick_quaternion_update(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz, float deltat);
void Madgwick_quaternion_angles(float *pitch, float *roll, float *yaw);

#endif /* MADGWICK_H_ */
