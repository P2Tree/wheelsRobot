#ifndef ATTITUDE_H_VJIKUSPZ
#define ATTITUDE_H_VJIKUSPZ

#define PI      3.14159265

void showQuaternion(void);

void showEuler(float y, float p, float r);

void calculateEuler(float *yaw, float *pitch, float *roll);

void *attitudeThread(void);



#endif /* end of include guard: ATTITUDE_H_VJIKUSPZ */
