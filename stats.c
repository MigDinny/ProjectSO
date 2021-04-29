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
            //printf("team: %d, car: %d; result: %d", t, c, intInArray(ignoreCarIDs, length, t*config.nCars + c));
            if (!intInArray(ignoreCarIDs, length, t*config.nCars + c)){

                if (carID == -1 || (cars[t*config.nCars + c].laps < cars[carID].laps) || 
                    (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos < cars[carID].pos)) {
                    //printf("LastID: %d == NewID: %d\n", carID, t*config.nCars + c);
                    carID = t*config.nCars + c;
                    
                }
            }
        }
    }

    return carID;
}

// guards the position of the last car to finish
void lastCar(int carID) {
        shmem->lastCarID = carID;
}

// add carID to carWIDs in shmem if there are still space
void addCarWIDs(int carID) {

    lastCar(carID);

    for (int i = 0; i < 5; i++) {
        if (shmem->carsWIDs[i] == -1) {
            shmem->carsWIDs[i] = carID;
            return;
        }
    }
}


// 20:05:59 > #1  CarNo: 1; Team: A; Laps: 10; Box stops: 4;
void stats(int mode) {
    char statsRace[MAX_COMMAND];
    printf("\n");
    plog("STATS:");
    
    int team_index, id;

    sem_wait(shmutex);
    
    // print 5 first car
    int loop;
    if (shmem->nCarsTotal > 5) {
        loop = 5;
    } else {
        loop = shmem->nCarsTotal;
    }


    int ignIDs[5];
    for (int i = 0; i < 5; i++) {
        ignIDs[i] = shmem->carsWIDs[i];
        //printf("teste: #%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d\n", i + 1, cars[ignIDs[i]].carNum, teams[ignIDs[i] / config.nCars].teamName, cars[ignIDs[i] ].laps, cars[ignIDs[i] ].boxStops);
    }


    for (int count = 0; count < loop; count++) {        // finished cars
        //printf("[%d] %d and %d\n", count, ignIDs[count], shmem->carsWIDs[count]);
        if (shmem->carsWIDs[count] != -1) {
            id = shmem->carsWIDs[count];
            team_index = id / config.nCars;
            // printf("%d  %d  %d  %d  %d\n", id, cars[id].carNum, cars[id].boxStops, cars[id].laps, cars[id].pos);
            sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", count + 1, cars[id].carNum, teams[team_index].teamName, cars[id].laps, cars[id].boxStops);

        } else {
            //printf("f: ");
            ignIDs[count] = find_first(ignIDs, 5);
            team_index = ignIDs[count] / config.nCars;
            sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", count + 1, cars[ignIDs[count]].carNum, teams[team_index].teamName, cars[ignIDs[count]].laps, cars[ignIDs[count]].boxStops);    
        }
        plog(statsRace);      
    } 


    // print last car
    if (shmem->nCarsTotal >= 6) {
        plog("...");
        if (shmem->runningCarsTotal == 0 && shmem->quitCars == 0) {
            // printf("race stopped\n");
            int lastID = shmem->lastCarID;
            team_index = lastID / config.nCars;
            sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", shmem->nCarsTotal, cars[lastID].carNum, teams[team_index].teamName, cars[lastID].laps, cars[lastID].boxStops);
            
        } else {
            int carID = find_last(ignIDs, 5);
            team_index = carID / config.nCars;
            // printf("LastID:%d   %d  %d  %d  %d  %d\n", carID, ignIDs[carID], cars[carID].carNum, cars[carID].boxStops, cars[carID].laps, cars[carID].pos);
            sprintf(statsRace, "#%d  CarNo: %d; Team: %s; Laps: %d; Box stops: %d;", shmem->nCarsTotal, cars[carID].carNum, teams[team_index].teamName, cars[carID].laps, cars[carID].boxStops);
        }
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

    sem_post(shmutex);
    
    printf("\n");

}
