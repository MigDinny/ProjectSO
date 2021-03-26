#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "car.h"


void *car_worker(void *id) {
    int my_id = *((int*) id);
    sleep(1);
    printf("car started [%d]\n", my_id);
    
    pthread_exit(NULL);

    return NULL;
}
