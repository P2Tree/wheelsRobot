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

static float sensor1Distance = 0.0;
static float sensor2Distance = 0.0;

void *cy30Thread() {
    int fd1, fd2;
#ifdef DEBUG
    int i;      // use to traverse read data
#endif
    wrBuffer dev1Buffer;
    wrBuffer dev2Buffer;

    DistanceContainer container1;
    DistanceContainer container2;

    fd1 = cy30Init(0);
    fd2 = cy30Init(1);
    if (fd1 > 0 && fd2 > 0)
        printf("cy30 init down. fd1 = %d, fd2 = %d\n", fd1, fd2);
    else {
        printf("cy30 init error.\n");
        pthread_exit((void*)1);
    }

    dev1Buffer.cmdlen = cy30ConstructCommand(Measure, 0x80, MeasureOnce, &dev1Buffer.command);
    dev2Buffer.cmdlen = cy30ConstructCommand(Measure, 0x81, MeasureOnce, &dev2Buffer.command);

    if (dev1Buffer.cmdlen > 0 && dev2Buffer.cmdlen > 0)
        printf("cy30 command construct down. cmdlen1 = %d, cmdlen2 = %d\n", dev1Buffer.cmdlen, dev2Buffer.cmdlen);
    else {
        printf("cy30 command construct error.\n");
        pthread_exit((void*)1);
    }

    while(1) {
        /* cy30GetDistance(fd1, &dev1Buffer, &container1, MeasureOnce); */
        /* sensor1Distance = container1.distance; */
        /* printf("CY30 sensor1: %.3f\n", container1.distance); */

        cy30GetDistance(fd2, &dev2Buffer, &container2, MeasureOnce);
        sensor2Distance = container2.distance;
        printf("CY30 sensor2: %.3f\n", container2.distance);
        
        // sensor 1


        // sensor 2
        /* // catch data */
        /* if (0 == cy30GetData(fd2, &dev2Buffer)) { */
            /* //process data */
/* #ifdef DEBUG */
            /* for (i=0; i<dev2Buffer.readlen; i++) */
                /* printf("%x ", dev2Buffer.readData[i]); */
            /* printf("\n"); */
/* #endif */
            /* if (0 == cy30ResultProcess(&container, dev2Buffer, MeasureOnce)) { */
                /* [> printf("address is 0x%02X, distance is %.3f\n", container.address, container.distance); <] */
                /* sensor2Distance = container.distance; */
                /* printf("CY30 sensor 2 address : 0x%02X, distance: %.3f\n", container.address, sensor2Distance); */
/*  */
            /* } */
            /* else */
                /* printf("CY30 sensor 1 distance error data\n"); */
            /* *dev2Buffer.readData = '\0'; */
        /* } */
        /* else */
            /* printf("CY30 sensor 2 distance no data\n"); */


    }
    free(dev1Buffer.command);
    free(dev2Buffer.command);
    close(fd1);
    close(fd2);
    return (void*)0;
}
