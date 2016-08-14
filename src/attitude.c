#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#include "attitude.h"
#include "gy953_main.h"
#include "mahony.h"

volatile static float senYaw, senPitch, senRoll;     // euler angle output from sensor dircetly

void *attitudeThread() {
    /* float accX, accY, accZ; */
    /* float gyrX, gyrY, gyrZ; */
    /* float magX, magY, magZ; */
    /* float radianGyrX, radianGyrY, radianGyrZ; */
    /* static float yaw, pitch, roll;      // euler angle calculate from quarternion in mahony algorithm */
    /* static float q0, q1, q2, q3;     // quarternion in mahony algorithm */

    while(1) {
        // mahony algorithm (but because of the P and I arguments in mahony algorithm adjusted not
        // very well, the last data of euler angle from mahony algorithm is not better than euler
        // angle output from sensor directly)
        // This is a TODO part for mahony algorithm.

        // get acc, gyr, mag data from sensor output, functions defined in gy953_main.c
        /* getAcc(&accX, &accY, &accZ); */
        /* getGyr(&gyrX, &gyrY, &gyrZ); */
        /* getMag(&magX, &magY, &magZ);  */
        /* showAcc(); */
        /* showGyr(); */
        /* showMag(); */
        // arguments of yaw pitch roll is unit of degress, should be convert to unit of radian
        /* radianGyrX = (gyrX/180)*PI; */
        /* radianGyrY = (gyrY/180)*PI; */
        /* radianGyrZ = (gyrZ/180)*PI; */
        /* MahonyAHRSupdateIMU(radianGyrX, radianGyrY, radianGyrZ, accX, accY, accZ); */
        /* MahonyAHRSupdateIMU(gyrX, gyrY, gyrZ, accX, accY, accZ); */
        /* MahonyAHRSupdate(gyrX, gyrY, gyrZ, accX, accY, accZ, magX, magY, magZ); */
        /* getQuaternion(&q0, &q1, &q2, &q3); */
        /* showQuaternion(); */
        /* calculateEuler(&yaw, &pitch, &roll); */
        /* showEuler(yaw, pitch, roll); */

        getEulerFromSensor(&senYaw, &senPitch, &senRoll);
        usleep(5000);   //5ms
        // TODO: calculate angle when a turn signal triggered
        //
    }
} // end of attitudeThread

/**
 * * Other local functions
 * */

/**
 * * Calculate function
 * */
// calculate euler from quaternion
void calculateEuler(float *yaw, float *pitch, float *roll) {
    *yaw = atan2(2*q1*q2 + 2*q0*q3, -2*q2*q2 - 2*q3*q3 + 1) * 57.3;  // yaw
    *pitch = asin(2*q1*q3 + 2*q0*q2) * 57.3;     //pitch
    *roll = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) * 57.3;    // roll
}

/**
 * * Show function
 * */
void showQuaternion() {
    printf("q0 = %.6f\t", q0);
    printf("q1 = %.6f\t", q1);
    printf("q2 = %.6f\t", q2);
    printf("q3 = %.6f\n", q3);
}

void showEuler(float y, float p, float r) {
    printf("yaw = %.6f\t", y);
    printf("pitch = %.6f\t", p);
    printf("roll = %.6f\n", r);
}
