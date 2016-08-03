/********************************************************
 *
 * @file    cy30_main.c
 * @brief   only to boot cy30 control.
 * @author  PWE dicksonliuming@gmail.com
 * @date    2016-7-16
 *
 * *******************************************************/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include "cy30_com.h"
#include "cy30_main.h"

void * cy30Thread() {
    int fd1, fd2;
    wrBuffer dev1Buffer;
    wrBuffer dev2Buffer;
    DistanceContainer container;

    printf("before init cy30\n");
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
        // catch data
        cy30DistanceMultiple(fd1, fd2, dev1Buffer, dev2Buffer);

        // process data
        if (0 == cy30ResultProcess(&container, dev1Buffer.readData, dev1Buffer.readlen, MeasureOnce)) {
            printf("address is 0x%02X , distance is %.3f\n", 
                    container.address, container.distance);
        }

        if (0 == cy30ResultProcess(&container, dev2Buffer.readData, dev2Buffer.readlen, MeasureOnce)) {
            printf("address is 0x%02X, distance is %.3f\n",
                    container.address, container.distance);
        }

        *dev1Buffer.readData = '\0';
        *dev2Buffer.readData = '\0';
    }
    close(fd1);
    close(fd2);
    return 0;
}
