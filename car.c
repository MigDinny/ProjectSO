/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


#include "include.h"


void *car_worker(void *id) {
    int my_id = *((int*) id);
    sleep(5);
    printf("car started [%d]\n", my_id);
    
    pthread_exit(NULL);

    return NULL;
}
