#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "gy953_main.h"

float origin_yaw = 0.0;

void *myThread2(void) {
    int i;
    for (i=0; i<100; i++) {
        printf("This is the cy30 pthread.\n");
        yaw = getYawAngle();
        printf("yaw = %.2f\n", yaw);
        sleep(1);
    }
    return (void *)0;
}

void *myThread3(void) {
    int i;
    for (i=0; i<100; i++) {
        printf("This is the ks103 pthread.\n");
        sleep(1);
    }
    return (void *)0;
}

void *myThread4(void) {
    int i;
    for (i=0; i<100; i++) {
        printf("robot localization calculate.\n");
        sleep(1);
    }
    return (void *)0;
}


int main(int argc, char *argv[]) {
    int pthread_ret = 0;
    pthread_t gy953ThreadId;
    pthread_t id2, id3, id4;

    pthread_ret = pthread_create(&gy953ThreadId, NULL, (void*)gy953Thread, NULL);
    if ( pthread_ret ) {
        printf("Create pthread error! gy953 thread.\n");
        return 1;
    }

    pthread_ret = pthread_create(&id2, NULL, (void*)myThread2, NULL);
    if ( pthread_ret ) {
        printf("Create pthread error!\n");
        return 1;
    }

    pthread_ret = pthread_create(&id3, NULL, (void*)myThread3, NULL);
    if ( pthread_ret ) {
        printf("Create pthread error!\n");
        return 1;
    }

    pthread_ret = pthread_create(&id4, NULL, (void*)myThread4, NULL);
    if ( pthread_ret ) {
        printf("Create pthread error!\n");
        return 1;
    }

    pthread_join(gy953ThreadId, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);
    pthread_join(id4, NULL);

    return 0;
}
