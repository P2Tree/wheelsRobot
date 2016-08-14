#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#include "attitude.h"
#include "gy953_main.h"
#include "mahony.h"

volatile static float angleYaw = 0.0;


void *attitudeThread() {
    float accX, accY, accZ;
    float gyrX, gyrY, gyrZ;
    float magX, magY, magZ;
    float radianGyrX, radianGyrY, radianGyrZ;
    static float yaw, pitch, roll;
    static float q0, q1, q2, q3;

    while(1) {
        
        getAcc(&accX, &accY, &accZ);
        getGyr(&gyrX, &gyrY, &gyrZ);
        getMag(&magX, &magY, &magZ); 
        /* showAcc(); */
        /* showGyr(); */

        // arguments of yaw pitch roll is unit of degress, should be convert to unit of radian
        radianGyrX = (gyrX/180)*PI;
        radianGyrY = (gyrY/180)*PI;
        radianGyrZ = (gyrZ/180)*PI;
        /* MahonyAHRSupdateIMU(radianGyrX, radianGyrY, radianGyrZ, accX, accY, accZ); */
        /* MahonyAHRSupdateIMU(gyrX, gyrY, gyrZ, accX, accY, accZ); */
        MahonyAHRSupdate(gyrX, gyrY, gyrZ, accX, accY, accZ, magX, magY, magZ);

        getQuaternion(&q0, &q1, &q2, &q3);
        showQuaternion();
        sleep(1);
        calculateEuler(&yaw, &pitch, &roll);
        /* showEuler(yaw, pitch, roll); */

    }
} // end of attitudeThread

void showQuaternion() {
    printf("q0 = %.6f ", q0);
    printf("q1 = %.6f ", q1);
    printf("q2 = %.6f ", q2);
    printf("q3 = %.6f\n", q3);
}

void calculateEuler(float *yaw, float *pitch, float *roll) {

    *yaw = atan2(2*q1*q2 + 2*q0*q3, -2*q2*q2 - 2*q3*q3 + 1) * 57.3;  // yaw
    *pitch = asin(2*q1*q3 + 2*q0*q2) * 57.3;     //pitch
    *roll = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) * 57.3;    // roll
}

void showEuler(float y, float p, float r) {
    printf("yaw = %.6f ", y);
    printf("pitch = %.6f ", p);
    printf("roll = %.6f\n", r);
}
