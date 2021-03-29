/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#include "include.h"

void team_manager_worker(shmem_t *shmem, int teamID) {

    pthread_t carThreadIds[shmem->config.nCars];           // move to shared memory
    int id[shmem->config.nCars];

    for (int i = 0; i < shmem->config.nCars; i++) {                    // change the number of cars to the number given by command line
        id[i] = i;
        printf("%d\n", i);
        pthread_create(&carThreadIds[i], NULL, car_worker, &id[i]);
    }

    for (int i = 0; i < shmem->config.nCars; i++) {     // change the number of cars to the number given by command line
        pthread_join(carThreadIds[i], NULL);
    }
}
