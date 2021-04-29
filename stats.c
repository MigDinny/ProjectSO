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

                if (carID == -1 || (cars[t*config.nCars + c].laps > cars[carID].laps) || 
                    (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos > cars[carID].pos)) {
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

                if (carID == -1 || (cars[t*config.nCars + c].laps < cars[carID].laps) || 
                    (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos < cars[carID].pos)) {
                    carID = t*config.nCars + c;
                }
            }
        }
    }

    return carID;
}

void verifyLastCar(int carID) {
    if ((cars[carID].laps < cars[shmem->lastCarID].laps) || (cars[carID].laps == cars[shmem->lastCarID].laps && cars[carID].pos < cars[shmem->lastCarID].pos)) {
        shmem->lastCarID = carID;
    }
}

// add carID to carWIDs in shmem if there are still space
// mode 0 -> car finished the race; mode 1 -> car quit race; mode 2 -> end race mide way
void addCarWIDs(int carID, int mode) {
    int aux;
    if (mode == 2) {
        for (int i = 0; i < 5; i++) {
            if (shmem->carsWIDs[i] == -1) {
                shmem->carsWIDs[i] = carID;
                return;
            } else if ((cars[carID].laps > cars[shmem->carsWIDs[i]].laps) || (cars[carID].laps == cars[shmem->carsWIDs[i]].laps && cars[carID].pos > cars[shmem->carsWIDs[i]].pos)) {
                aux = shmem->carsWIDs[i];
                shmem->carsWIDs[i] = carID;
                carID = aux;
            }
        }
    }
    
    if (mode == 1) 
        verifyLastCar(carID);

    if (mode == 0) {
        for (int i = 0; i < 5; i++) {
            if (shmem->carsWIDs[i] == -1) {
                shmem->carsWIDs[i] = carID;
                return;
            }
        }
    }
}


// 20:05:59 > #1  CarNo: 1; Team: A; Laps: 10; Box stops: 4;
void stats(int mode) {
    char statsRace[MAX_COMMAND];
    printf("\n");
    plog("STATS:");
    
    int team_index, id;


    
    // print 5 first car
    int loop;
    if (shmem->nCarsTotal > 5) {
        loop = 5;
    } else {
        loop = shmem->nCarsTotal;
    }

    sem_wait(shmutex);

    int ignIDs[5];
    for (int i = 0; i < 5; i++) {
        ignIDs[i] = shmem->carsWIDs[i];

    }

    for (int count = 0; count < loop; count++) {
        //printf("[%d] %d and %d\n", count, ignIDs[count], shmem->carsWIDs[count]);

        if (shmem->carsWIDs[count] != -1) {      // finished cars
            id = shmem->carsWIDs[count];
            team_index = id / config.nCars;
            sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", count + 1, cars[id].carNum, teams[team_index].teamName, cars[id].laps, cars[id].boxStops);

        } else {                                // cars still running
            ignIDs[count] = find_first(ignIDs, 5);
            team_index = ignIDs[count] / config.nCars;
            sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", count + 1, cars[ignIDs[count]].carNum, teams[team_index].teamName, cars[ignIDs[count]].laps, cars[ignIDs[count]].boxStops);    
        }
        plog(statsRace);
    }

    // print last car
    if (shmem->nCarsTotal >= 6) {
        plog("...");
        int arr[] = {};
        int carID = find_last(arr, 0);
        sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", shmem->nCarsTotal, cars[carID].carNum, teams[team_index].teamName, cars[carID].laps, cars[carID].boxStops);
        plog(statsRace);

    }
    sem_post(shmutex);
    
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
