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

/* including sensor distance (container.distance) and sensor address (container.address) */
DistanceContainer container1 = {SENSORADDRESS1, 0};

void *cy30Thread() {
    int fd;
    const char *port = "/dev/ttymxc3";

#ifdef DEBUG_CY30
    printf("cy30 debug mode is on\n");
#endif

    /* cy30 initialize */
    /* including open uart port; construct command; */
    fd = cy30Init(port);
    if (fd > 0) {
        printf("cy30 init down. port: %s, fd = %d\n", port, fd);
    }
    else if (-1 == fd) {
        printf("cy30 init open port error.\n");
        pthread_exit((void*)1);
    }
    else if (-2 == fd) {
        printf("cy30 init create command error.\n");
        pthread_exit((void*)1);
    }

    /* LOOP */
    while(1) {
        // get sensor 1 distance
        if ( 0 != cy30GetDistance(fd, &container1, MeasureOnce)) {
            continue;
        }
        printf("CY30 sensor: %.3f\n", container1.distance);

    }
    close(fd);
    return (void*)0;
}
