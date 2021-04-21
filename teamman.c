/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <pthread.h>
#include <stdio.h>
#include <signal.h>

#include "include.h"

void team_manager_worker(int teamID) {

    // <<< DEBUG >>>
    signal(SIGINT, SIG_DFL);

    pthread_t carThreadIds[config.nCars];           // move to shared memory
    int id[config.nCars];

    sprintf(teams[teamID].teamName, "%s%d", "team", teamID);

    for (int i = 0; i < teams[teamID].nCars; i++) {                    // change the number of cars to the number given by command line
        id[i] = teamID*config.nCars + i;

        pthread_create(&carThreadIds[i], NULL, car_worker, &id[i]);
    }

    for (int i = 0; i < teams[teamID].nCars; i++) {     // change the number of cars to the number given by command line
        pthread_join(carThreadIds[i], NULL);
    }
}
