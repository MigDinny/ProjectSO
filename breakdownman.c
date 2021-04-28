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

void startBreakDown() {
    return;
}

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
    char pCommand[MAX_COMMAND];
    message_t failure;

    struct sigaction act;
 
	memset (&act, '\0', sizeof(act));
 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &startBreakDown;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
 
	sigaction(SIGUSR2, &act, NULL);

    pause(); // wait for signal

    while (1) {
        
        if (shmem->status == OFF)
            pause();

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
                    sprintf(pCommand, "PROBLEM SENT TO CAR [%d]", cars[tempid].carNum);
                    plog(pCommand);
                }
            }
            
        }

        // sleep until next calculation - delay is in seconds | * 1000 to convert to ms * 1000 to convert to us * multiplier 
        usleep(delay * 1000 * 1000 * multiplier);
    }

}