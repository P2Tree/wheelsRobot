#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#include "ks103_com.h"
#include "ks103_main.h"

/**
 * *    PROTECTED LOCAL PARAMATER
 * */

// sensor 1 distance, range to ?, unit is 'mm'
static int distanceOfSensor1 = 0;
// sensor 2 distance, range to ?, unit is 'mm'
static int distanceOfSensor2 = 0;
// sensor 3 distance, range to ?, unit is 'mm'
static int distanceOfSensor3 = 0;
// sensor 4 distance, range to ?, unit is 'mm'
static int distanceOfSensor4 = 0;

void *ks103Thread(){
    int fd;
    int distanceResult[4];

    fd = KS103Init();
    if (-1 == fd) {
        printf("KS103 init fault.\n");
        pthread_exit((void*)1);
    }
    printf("KS103 init down.\n");

    while(1) {
        distanceMultipleKS103(fd, KS103ADDRESS_1, 4, distanceResult);

        distanceOfSensor1 = distanceResult[0];
        distanceOfSensor2 = distanceResult[1];
        distanceOfSensor3 = distanceResult[2];
        distanceOfSensor4 = distanceResult[3];

        showKS103Distance();
        sleep(1);
    }

    distanceAKS103(fd, (const char *)KS103ADDRESS_1);

    /* fprintf(stderr, USAGE_MESSAGE, argv[0], argv[0], argv[0], argv[0]); */

    close(fd);
    return 0;
}

void showKS103Distance() {
    printf("KS103 1: %dmm ", KS103ADDRESS_1, distanceOfSensor1);
    printf("KS103 2: %dmm ", KS103ADDRESS_2, distanceOfSensor2);
    printf("KS103 3: %dmm ", KS103ADDRESS_3, distanceOfSensor3);
    printf("KS103 4: %dmm ", KS103ADDRESS_4, distanceOfSensor4);
    printf("\n");
}

