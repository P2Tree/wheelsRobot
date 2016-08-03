#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "gy953_main.h"
#include "ks103_main.h"
#include "cy30_main.h"

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
    pthread_t ks103ThreadId;
    pthread_t cy30ThreadId;
    pthread_t id4;

    char *arg1 = argv[1];

    if (1 == argc || (2 == argc && !strcmp(arg1, "-ks103"))) {
        pthread_ret = pthread_create(&ks103ThreadId, NULL, (void*)ks103Thread, NULL);
        if ( pthread_ret ) {
            printf("Create pthread error! ks103 thread.\n");
            return 1;
        }
    }

    if (1 == argc || (2 == argc && !strcmp(arg1, "-gy953"))) {
        pthread_ret = pthread_create(&gy953ThreadId, NULL, (void*)gy953Thread, NULL);
        if ( pthread_ret ) {
            printf("Create pthread error! gy953 thread.\n");
            return 1;
        }
    }

    if (1 == argc || (2 == argc && !strcmp(arg1, "-cy30"))) {
        pthread_ret = pthread_create(&cy30ThreadId, NULL, (void*)cy30Thread, NULL);
        if ( pthread_ret ) {
            printf("Create pthread error! cy30 thread\n");
            return 1;
        }
    }

    if (1 == argc || (2 == argc && !strcmp(arg1, "-infrared"))) {
        pthread_ret = pthread_create(&id4, NULL, (void*)myThread4, NULL);
        if ( pthread_ret ) {
            printf("Create pthread error!\n");
            return 1;
        }
    }

    if ( argc > 2) {
        printf("You input wrong arguments, Please try again.\n");
        return -1;
    }

    /* main thread will be wait other son threads to end, all of other son threads are detachable */
    for(;;);

    return 0;
}

