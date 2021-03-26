#include <pthread.h>

#include "teamman.h"
#include "car.h"


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
