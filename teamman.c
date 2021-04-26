/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "include.h"

static void hdl (int sig, siginfo_t *siginfo, void *context)
{
	close(channel[1]);
    exit(0);
}


void terminate_teamman(int teamID, pthread_t carThreadIds[]) {

	// wait for all threads to finish (eventually) (join returns immediately if already exited)
    for (int i = 0; i < teams[teamID].nCars; i++) { 
        printf("WAITING CAR %d\n", i);
        pthread_join(carThreadIds[i], NULL);
    }


	// cleanup
    //close(channel[1]);
	pthread_mutex_destroy(&tc_mutex);
	pthread_cond_destroy(&in_box);
	pthread_cond_destroy(&out_box);
	
    printf("exit\n");


    struct sigaction act;
 
	memset (&act, '\0', sizeof(act));
 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &hdl;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
 
	sigaction(SIGTERM, &act, NULL);

    pause(); // wait for signal
	
}

void team_manager_worker(int teamID) {

    // <<< DEBUG >>>
    signal(SIGINT, SIG_DFL);

    // init mutexes and conds
    pthread_mutex_init(&tc_mutex, NULL);
    pthread_cond_init(&in_box, NULL);
    pthread_cond_init(&out_box, NULL);

    // init some control variables
    pthread_t carThreadIds[config.nCars];           // move to shared memory
    int id[config.nCars];
    awaitingCars = 0;
    awaitingSafetyCars = 0;
    teams[teamID].status = FREE;
	runningCars = teams[teamID].nCars;

    for (int i = 0; i < teams[teamID].nCars; i++) {  
        id[i] = teamID*config.nCars + i;
        pthread_create(&carThreadIds[i], NULL, car_worker, &id[i]);
    }

    // loop to manage box
    while (1) {
        pthread_mutex_lock(&tc_mutex);

        while (awaitingCars == 0 && runningCars > 0)
            pthread_cond_wait(&in_box, &tc_mutex); // @TODO consider timed_wait, might be stuck forever
        
        // check again because this iteration might be outdated because cond_wait blocked
        if (runningCars == 0) break;

        printf("aC = %d  car = %d  \n",awaitingCars, boxCarIndex);
        teams[teamID].status = BUSY;

        cars[boxCarIndex].fuel = config.fuelTank;
        teams[teamID].countRefuels++;
        
        pthread_mutex_unlock(&tc_mutex);

        // sleep 2 time units
        usleep(2 * 1000 * 1000 * config.multiplier);

        pthread_mutex_lock(&tc_mutex);

        teams[teamID].status = FREE;
    
        if (awaitingSafetyCars > 1)
            teams[teamID].status = RESERVED;


        pthread_cond_signal(&out_box);
        pthread_mutex_unlock(&tc_mutex);

    }

	terminate_teamman(teamID, carThreadIds);

}
