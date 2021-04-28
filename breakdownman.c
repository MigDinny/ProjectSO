/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#include "include.h"

void breakdown_manager_worker() {
    dlog("BREAKDOWN MANAGER STARTED");

    int delay = config.tBreakDown;
    float multiplier = config.multiplier;

    time_t t;
    srand((unsigned) time(&t));
    int i = 0;
    int j = 0;
    int randnum = 0;
    int tempid;
    message_t failure;

    while (1) {
        if (shmem->status == ON) {
            for (i = 0; i < shmem->nTeams; i++) {
                for (j = 0; j < teams[i].nCars; j++) {
                    // check each individual car's RELIABILITY
                    randnum = rand() % 100;
                    tempid = i*config.nCars + j;

                    // only generate failure if it hasn't already a failure and if not finished
                    if (randnum >= cars[tempid].reliability && cars[tempid].status != SAFETY && cars[tempid].status != NO_FUEL && cars[tempid].status != FINISHED && cars[tempid].status != BOX) {
                        // GENERATE FAILURE!
                        failure.mtype = tempid + 1;
                        msgsnd(shmem->mqid, &failure, sizeof(message_t), 0);
                    }
                }
            }
        }

        // sleep until next calculation - delay is in seconds | * 1000 to convert to ms * 1000 to convert to us * multiplier 
        usleep(delay * 1000 * 1000 * multiplier);
    }

}