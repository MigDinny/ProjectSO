#include <pthread.h>

#include "teamman.h"
#include "car.h"



void team_manager_worker(config_t config, shmem_t *shmem, sem_t *mutex) {

    pthread_t cars[config.nCars];           // move to shared memory
    int id[config.nCars];

    for (int i = 0; i < config.nCars; i++) {                    // change the number of cars to the number given by command line
        pthread_create(&cars[i], NULL, car_worker, &id[i]);
    }

    for (int i = 0; i < config.nCars; i++) {
        pthread_join(cars[i], NULL);
    }
}
