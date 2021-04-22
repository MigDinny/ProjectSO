/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "include.h"

int channel[2];

void team_manager_worker(int teamID) {

    // <<< DEBUG >>>
    signal(SIGINT, SIG_DFL);

    pthread_t carThreadIds[config.nCars];           // move to shared memory
    int id[config.nCars];

    sprintf(teams[teamID].teamName, "%s%d", "team", teamID);

    u_pipe_t team_comms;
    for (int i = 0; i < 10; i++) {
        sprintf(team_comms.text, "test team manager - %d", teamID);
        write(channel[1], &team_comms, sizeof(team_comms));
        sleep(1);
    }

    for (int i = 0; i < config.nCars; i++) {                    // change the number of cars to the number given by command line
        id[i] = teamID*config.nCars + i;

        pthread_create(&carThreadIds[i], NULL, car_worker, &id[i]);
    }

    for (int i = 0; i < config.nCars; i++) {     // change the number of cars to the number given by command line
        pthread_join(carThreadIds[i], NULL);
    }
}
