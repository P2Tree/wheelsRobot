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

void *ks103Thread(char workMode){
    int fd;
    int distanceResult[4];

    fd = KS103Init();
    if (-1 == fd) {
        printf("KS103 init fault.\n");
        pthread_exit((void*)1);
    }
    printf("KS103 init down.\n");

    while(1) {
        distanceMultipleKS103(fd, (const char *)KS103ADDRESS_1, (const char *)4, distanceResult);

        distanceOfSensor1 = distanceResult[0];
        distanceOfSensor2 = distanceResult[1];
        distanceOfSensor3 = distanceResult[2];
        distanceOfSensor4 = distanceResult[3];

        if ('k' == workMode)
            showKS103Distance();
    }

    distanceAKS103(fd, (const char *)KS103ADDRESS_1);

    /* fprintf(stderr, USAGE_MESSAGE, argv[0], argv[0], argv[0], argv[0]); */

    close(fd);
    return 0;
}

void showKS103Distance() {
    printf("KS103_1 sensor distance is: %d\n", distanceOfSensor1);
    printf("KS103_2 sensor distance is: %d\n", distanceOfSensor2);
    printf("KS103_3 sensor distance is: %d\n", distanceOfSensor3);
    printf("KS103_4 sensor distance is: %d\n", distanceOfSensor4);
}

