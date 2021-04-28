/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdlib.h>
#include <stdio.h>


#include "include.h"


char intInArray(int array[], int length, int num) {
    // check the array for the number given
    for (int i = 0; i < length; i++) {
        if (array[i] == num) 
            return 1;
    }
    return 0;
}


// gets the car in 1º position ignoring the carsIDs given in the array
int find_first (int ignoreCarIDs [], int length) {
    int carID = -1;

    for (int t = 0; t < shmem->nTeams; t++) {
        for (int c = 0; c < teams[t].nCars; c++) {
            if (!intInArray(ignoreCarIDs, length, t*config.nCars + c)){

                if (carID == -1) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps > cars[carID].laps) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos > cars[carID].pos) {
                    carID = t*config.nCars + c;
                }
            }
        }
    }

    return carID;
}

// gets the car in last position ignoring the carsIDs given in the array
int find_last (int ignoreCarIDs [], int length) {
    int carID = -1;

    for (int t = 0; t < shmem->nTeams; t++) {
        for (int c = 0; c < teams[t].nCars; c++) {
            if (!intInArray(ignoreCarIDs, length, t*config.nCars + c)){

                if (carID == -1) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps < cars[carID].laps) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos < cars[carID].pos) {
                    carID = t*config.nCars + c;
                }
            }
        }
    }

    return carID;
}


// 20:05:59 > #1  CarNo: 1; Team: A; Laps: 10; Box stops: 4;
void stats(int mode) {
    char statsRace[MAX_COMMAND];
    printf("\n");
    plog("STATS:");
    
    int carsIDs [5] = {-1, -1, -1, -1, -1};

    int team_index;
    
    // print 5 first car
    int loop;
    if (shmem->nCarsTotal > 5) {
        loop = 5;
    } else {
        loop = shmem->nCarsTotal;
    }

    for (int i = 0; i < loop; i++) {
        carsIDs[i] = find_first(carsIDs, sizeof(carsIDs) / sizeof(int));
    }

    for (int i = 0; i < loop; i++) {
        team_index = carsIDs[i] / config.nCars;
        sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", i + 1, cars[carsIDs[i]].carNum, teams[team_index].teamName, cars[carsIDs[i]].laps, cars[carsIDs[i]].boxStops);
        plog(statsRace);
    }

    // print last car
    if (shmem->nCarsTotal >= 6) {
        int carsIDsL [0];
        int lastID = find_last(carsIDsL, 0);
        team_index = lastID / config.nCars;

        plog("...");
        sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", shmem->nCarsTotal, cars[lastID].carNum, teams[team_index].teamName, cars[lastID].laps, cars[lastID].boxStops);
        plog(statsRace);
    }

    sprintf(statsRace, "Number of refuels: %d", shmem->countRefuels);
    plog(statsRace);
    sprintf(statsRace, "Number of breakdowns: %d", shmem->countBreakDowns);
    plog(statsRace);

    if (mode == 0)
        plog("Current number of cars in lane: 0");
    else {
        sprintf(statsRace, "Current number of cars in lane: %d", shmem->runningCarsTotal);
        plog(statsRace); 
    }
    printf("\n");

}
