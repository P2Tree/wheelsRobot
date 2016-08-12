#ifndef GY953_MAIN_H_D0MAET5W
#define GY953_MAIN_H_D0MAET5W

#define GY953ACCTRANS       16383
#define GY953GYRTRANS       16.4
#define GY953MAGTRANS       6.7

float getYawAngle(void);

void showEulerAngle(void);

void showAcc(void);

void showGyr(void);

void *gy953Thread(void);

#endif /* end of include guard: GY953_MAIN_H_D0MAET5W */
