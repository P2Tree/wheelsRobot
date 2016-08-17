/********************************************************
 *
 * @file    cy30_main.c
 * @brief   only to boot cy30 control.
 * @author  PWE dicksonliuming@gmail.com
 * @date    2016-7-16
 *
 * *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include "cy30_com.h"
#include "cy30_main.h"

// sensor distance result
volatile static float sensorDistance = 0.0;

void *cy30Thread() {
    int fd;
    const char *port = "/dev/ttymxc3";

#ifdef DEBUG_CY30
    printf("debug mode is on\n");
#endif
    wrBuffer devBuffer;

    DistanceContainer container;
    int i;

    fd = cy30Init(port, &devBuffer);
    if (fd > 0) {
        printf("cy30 init down. port: %s, fd = %d\n", port, fd);
        printf("cy30 command construct down. cmdlen = %d\n", devBuffer.cmdlen);
    }
    else if (-1 == fd) {
        printf("cy30 init open port error.\n");
        pthread_exit((void*)1);
    }
    else if (-2 == fd) {
        printf("cy30 init create command error.\n");
        pthread_exit((void*)1);
    }

    while(1) {
        if ( 0 != cy30GetDistance(fd, &devBuffer, &container, MeasureOnce)) {
            continue;
        }
        sensorDistance = container.distance;
        printf("CY30 sensor: %.3f\n", container.distance);

    }
    free(devBuffer.command);
    close(fd);
    return (void*)0;
}
