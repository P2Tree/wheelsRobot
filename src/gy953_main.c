#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "gy953_main.h"
#include "gy953_com.h"
#include <math.h>
#include "mahony.h"

/**
 * three axis euler angles 
 * In this project, we only use angle_z to check robot direction
 * */

// Please attention for mutex

/* x is alpha angle, roll angle, value from -PI to PI */
volatile static float angle_x = 0.0;
/* y is gamma angle, pitch angle, value from -1/2PI to 1/2PI */
volatile static float angle_y = 0.0;
/* z is beta angle, yaw angle, value from -PI to PI */
volatile static float angle_z = 0.0;

/**
 * three axis accelerometer data.
 * accelerometer and gyroscope in the follow will used to join in
 * hamony algorithm.
 * */
/* x accel is value from -2g to 2g, normal is 0g in stationary state*/
volatile static float acc_x = 0.0;
/* y accel is value from -2g to 2g, normal is 0g in stationary state*/
volatile static float acc_y = 0.0;
/* z accel is value from -2g to 2g, normal is -1g in stationary state*/
volatile static float acc_z = 0.0;

/* accelerometer data static shift offset value, should measure them
 * in a statical and stability state.*/
 static float accOffset_x = -0.006537;
 static float accOffset_y = -0.105023;
 static float accOffset_z = -0.006095;

/**
 * three axis gyroscope data.
 * */
/* x gyro is value from -2000dps to 2000dps, normal is 0dps in stationary state */
volatile static float gyr_x = 0.0;
/* y gyro is value from -2000dps to 2000dps, normal is 0dps in stationary state */
volatile static float gyr_y = 0.0;
/* z gyro is value from -2000dps to 2000dps, normal is 0dps in stationary state */
volatile static float gyr_z = 0.0;

/* same as accelerometer offset data */
 static float gyrOffset_x = 0.0;
 static float gyrOffset_y = 0.0;
 static float gyrOffset_z = 0.0;

// command arguments
static unsigned char eulerCommand[3];
static unsigned char accCommand[3];
static unsigned char gyrCommand[3];
static unsigned char magCommand[3]; 

void *gy953Thread() {
    int fd;
    signed short result[3];

    const char *port = "/dev/ttymxc1";
    /* unsigned char eulerCommand[3]; */

    fd = gy953Init(port);
    if (-1 == fd) {
        printf("GY953 init fault.\n");
        pthread_exit((void*)1);
    }
    gy953ConstructCommand(EULERANGLE, eulerCommand);
    gy953ConstructCommand(ACCELEROMETER, accCommand);
    gy953ConstructCommand(GYROSCOPE, gyrCommand);
    gy953ConstructCommand(MAGNETOMETER, magCommand);

    printf("GY953 init down. port is %s, fd = %d\n", port, fd);

    while(1) {
        /* getGY953Result(fd, 1, eulerCommand, result); */
        /* angle_x = angleResult[0]; */
        /* angle_y = angleResult[1]; */
        /* angle_z = angleResult[2]; */
        /* showEulerAngle(); */

        getGY953Result(fd, 2, accCommand, result);
        acc_x = (result[0] / (float)GY953ACCTRANS) - accOffset_x;   // GY953ACCTRANS = 16383
        acc_y = (result[1] / (float)GY953ACCTRANS) - accOffset_y;
        acc_z = (result[2] / (float)GY953ACCTRANS) - accOffset_z;
        /* getAccAve(result[0] / (float)GY953ACCTRANS, result[1] / (float)GY953ACCTRANS, result[2] / (float)GY953ACCTRANS); */
        printf("acc: ");
        showOrigin(result[0], result[1], result[2]);
        result[0] = 0;
        result[1] = 0;
        result[2] = 0;
        /* showAcc(); */

        getGY953Result(fd, 2, gyrCommand, result);
        gyr_x = (result[0] / (float)GY953GYRTRANS) - gyrOffset_x;     // 16.4 is a transfer const arg in gy953 gyroscope sensor parts
        gyr_y = (result[1] / (float)GY953GYRTRANS) - gyrOffset_y;
        gyr_z = (result[2] / (float)GY953GYRTRANS) - gyrOffset_z;
        printf("gyr: ");
        showOrigin(result[0], result[1], result[2]);
        result[0] = 0;
        result[1] = 0;
        result[2] = 0;
        /* showGyr(); */
        sleep(1);
        /* usleep(10000);   // 10ms */
    }
    gy953Close(fd);
    return (void *)0;
}

float getYawAngle(void) {
    return angle_z;
}

void getAcc(float *retAccX, float *retAccY, float *retAccZ) {
    *retAccX = acc_x;
    *retAccY = acc_y;
    *retAccZ = acc_z;
}

void getGyr(float *retGyrX, float *retGyrY, float *retGyrZ) {
    *retGyrX = acc_x;
    *retGyrY = acc_y;
    *retGyrZ = acc_z;
}

void showEulerAngle(void) {
    printf("angle x: %.6f ", angle_x);
    printf("angle y: %.6f ", angle_y);
    printf("angle z: %.6f\n", angle_z);
}

void showAcc(void) {
    printf("acceleration x: %.6f ", acc_x);
    printf("acceleration y: %.6f ", acc_y);
    printf("acceleration z: %.6f\n", acc_z);
}

void showOrigin(float originx, float originy, float originz) {
    printf("origin x: %.6f ", originx);
    printf("origin y: %.6f ", originy);
    printf("origin z: %.6f\n", originz);
}

void showGyr(void) {
    printf("gyroscope x = %.6f ", gyr_x);
    printf("gyroscope y = %.6f ", gyr_y);
    printf("gyroscope z = %.6f\n", gyr_z);
}

void getAccAve(float acc_x, float acc_y, float acc_z) {
    static float addx = 0.0;
    static float addy = 0.0;
    static float addz = 0.0;

    static int n = 0;
    n++;

    addx += acc_x;
    addy += acc_y;
    addz += acc_z;

    printf("average accelerometer x: %.6f ", addx/n);
    printf("average accelerometer y: %.6f ", addy/n);
    printf("average accelerometer z: %.6f\n", addz/n);
    // accelerometer z axis must add 1.0 to result offset z, 1g gravitational acceleration
    // should be consider.

    // TODO: auto amend accelerometer
}
