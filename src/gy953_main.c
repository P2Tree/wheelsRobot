#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "gy953_main.h"
#include "gy953_com.h"

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


void *gy953Thread() {
    int fd;
    float angleResult[3];

    const char *port = "/dev/ttymxc1";
    unsigned char command[3];

    fd = gy953Init(port, command);
    if (-1 == fd) {
        printf("GY953 init fault.\n");
        pthread_exit((void*)1);
    }
    printf("GY953 init down. port is %s, fd = %d\n", port, fd);

    while(1) {

        get3AxisEulerAngle(fd, command, angleResult);
        
        angle_x = angleResult[0];
        angle_y = angleResult[1];
        angle_z = angleResult[2];

        sleep(1);
        /* usleep(10000);   // 10ms */
        showEulerAngle();
    }
    gy953Close(fd);
    return (void *)0;
}

float getYawAngle(void) {
    return angle_z;
}

void showEulerAngle(void) {
    printf("angle x: %.2f ", angle_x);
    printf("angle y: %.2f ", angle_y);
    printf("angle z: %.2f\n", angle_z);
}
