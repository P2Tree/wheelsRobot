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
#ifdef DEBUG
    int i;      // use to traverse read data
#endif
    wrBuffer devBuffer;

    DistanceContainer container;

    fd = cy30Init(0);
    if (fd > 0)
        printf("cy30 init down. fd = %d\n", fd);
    else {
        printf("cy30 init error.\n");
        pthread_exit((void*)1);
    }

    devBuffer.cmdlen = cy30ConstructCommand(Measure, 0x80, MeasureOnce, &devBuffer.command);

    if (devBuffer.cmdlen > 0)
        printf("cy30 command construct down. cmdlen = %d\n", devBuffer.cmdlen);
    else {
        printf("cy30 command construct error.\n");
        pthread_exit((void*)1);
    }

    while(1) {
        cy30GetDistance(fd, &devBuffer, &container, MeasureOnce);
        sensorDistance = container.distance;
        printf("CY30 sensor: %.3f\n", container.distance);

    }
    free(devBuffer.command);
    close(fd);
    return (void*)0;
}
