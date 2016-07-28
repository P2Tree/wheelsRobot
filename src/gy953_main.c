#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "gy953_main.h"
#include "gy953_com.h"

//extern void uartInit();
/**
 * three axis euler angles 
 * In this project, we only use angle_z to check robot direction
 * */

/* x is alpha angle, roll angle, value from -PI to PI */
static float angle_x = 0.0;
/* y is gamma angle, pitch angle, value from -1/2PI to 1/2PI */
static float angle_y = 0.0;
/* z is beta angle, yaw angle, value from -PI to PI */
static float angle_z = 0.0;

void *gy953Thread() {
    int fd;
    int i;
    unsigned char workMode;
    unsigned char recData[MAXLEN];
    int receiveLen = 0;
    unsigned char sendData[WRITELEN] = "\0"; 
    int sendLen = 0;

    fd = gy953Init(sendData);
    if (-1 == fd) {
        printf("GY953 init fault.\n");
        pthread_exit((void*)1);
    }
    printf("GY953 init down.\n");
    while(1) {
        sendLen = gy953SendCommand(fd, sendData, WRITELEN);
        if (-1 == sendLen) {
            printf("sendLen = %d", sendLen);
            printf("send error\n");
            for (i=0; i<sendLen; i++)
                printf("%02x ", sendData[i]);
            printf("\n---\n");
            continue;
        }
        /* printf("send.\n"); */
        sleep(1);
        receiveLen = gy953ReceiveData(fd, recData, MAXLEN);
        if ( -1 == receiveLen) {
            printf("receive error\n");
            printf("receiveLen: %d\n", receiveLen);
            for (i=0; i<receiveLen; i++)
                printf("%02x ", recData[i]);
            printf("\n---\n");
            continue;
        }
        /* printf("receive.\n"); */
        analysisEulerangle(recData, receiveLen, &angle_x, &angle_y, &angle_z);
        /* WORK MODE */
        workMode = 'n';
        switch (workMode) {
            case 'g': showEulerAngle();
                      break;
            case 'n': break;
            default : break;
        }
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
