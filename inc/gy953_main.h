#ifndef GY953_MAIN_H_D0MAET5W
#define GY953_MAIN_H_D0MAET5W

#define GY953ACCTRANS       16383
#define GY953GYRTRANS       16.4
#define GY953MAGTRANS       6.7

float getYawAngle(void);

void getAcc(float *retAccX, float *retAccY, float *retAccZ);

void getGyr(float *retGyrX, float *retGyrY, float *retGyrZ);

void showEulerAngle(void);

void showAcc(void);

void showAccOrigin(float originx, float originy, float originz);

void showGyr(void);

void getAccAve(float acc_x, float acc_y, float acc_z);

void *gy953Thread(void);

#endif /* end of include guard: GY953_MAIN_H_D0MAET5W */
