#ifndef GY953_MAIN_H_D0MAET5W
#define GY953_MAIN_H_D0MAET5W

float getYawAngle(void);

void showEulerAngle(void);

void showAcc(void);

void showGyr(void);

int offsetCheckAcc(float accx, float accy, float accz, float *aveAccx, float *aveAccy, float *aveAccz);

void *gy953Thread(void);

#endif /* end of include guard: GY953_MAIN_H_D0MAET5W */
