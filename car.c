/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "include.h"

void finish(int my_index) {
	
	char pCommand[MAX_COMMAND];
	sprintf(pCommand, "[%d] ending", cars[my_index].carNum);
	dlog(pCommand);

	// add carID to array of winners
	// minus one car
	sem_wait(shmutex);
	runningCarsT--;
	shmem->runningCarsTotal--;
	sem_post(shmutex);
	
	// [Unnamed Pipe] signal raceman: this car finished
	command_t send;
	send.carID = my_index;
	send.carStatus = FINISHED;
	write(channel[1], &send, sizeof(command_t));
	
	// if last car, unblock team-manager (it is stuck on while loop)
	if (runningCarsT == 0)
		pthread_cond_signal(&in_box);
	
	pthread_exit(NULL);
}

void *car_worker(void *car_index) {
	char pCommand[MAX_COMMAND];
    int my_index = *((int*) car_index);
	int team_index = my_index / config.nCars;

	// minimum fuel for 1, 2 or 4 laps
	float fuel1 = cars[my_index].consumption * config.distance / cars[my_index].speed;
    float fuel2 = 2 * fuel1;
	float fuel4 = 4 * fuel1;
	int tryBox = 0; // 0 false 1 true
	cars[my_index].boxStops = 0;
	
	message_t receivedMSG;
	int receivedBytes = 0;
	int isCarFailure = 0;

	// each iteration is a TIME UNIT
	while(1) {

		// try to read a message from MQ; NO BLOCK
		receivedBytes = msgrcv(shmem->mqid, &receivedMSG, sizeof(message_t), my_index+1, IPC_NOWAIT);

		if (receivedBytes > 0) {
			sprintf(pCommand, "NEW PROBLEM IN CAR %d", cars[my_index].carNum);
			plog(pCommand);

			cars[my_index].status = SAFETY;
			sprintf(pCommand, "[%d] changed status to SAFETY", cars[my_index].carNum);
			plog(pCommand);
			isCarFailure = 1;
			receivedBytes = 0;
			tryBox = 1;
			shmem->countBreakDowns++;
		}

		// proccess a time unit -> update positions
		if (cars[my_index].status == SAFETY) {
			cars[my_index].pos += 0.3*cars[my_index].speed;
			cars[my_index].fuel -= 0.4*cars[my_index].consumption;
		} else {
			cars[my_index].pos += cars[my_index].speed;
			cars[my_index].fuel -= cars[my_index].consumption;
		}
		

		// new lap
		if (cars[my_index].pos >= config.distance) {
			// update vars
			cars[my_index].pos -= config.distance;
			cars[my_index].laps++;
			
			// if finished race
			if (cars[my_index].laps >= config.nTurns) {
				cars[my_index].status = FINISHED;
				sprintf(pCommand, "[%d] changed status to FINISHED", cars[my_index].carNum);
				plog(pCommand);
				sprintf(pCommand, "[%d] finished\n", cars[my_index].carNum);
				dlog(pCommand);

				sem_wait(shmutex);
				addCarWIDs(my_index, 0);
				sem_post(shmutex);
				
				finish(my_index);
			}
			
			// if this car is trying to get into box
			if (tryBox == 1) {
				// we are in box, acquire lock 
				pthread_mutex_lock(&tc_mutex);

				// check if box is empty
				if ( (teams[team_index].status == FREE) || (teams[team_index].status == RESERVED && cars[my_index].status == SAFETY) ) {
					// change these variables so the condition breaks on the other side and it stops looping on cond_wait()
					awaitingCars++;
					if (cars[my_index].status == SAFETY)
						awaitingSafetyCars++;

					boxCarIndex = my_index;

					sprintf(pCommand, "[%d] in box", cars[my_index].carNum);
					dlog(pCommand);

					if (isCarFailure == 1) {
						isTeamCarFailure = 1;
						isCarFailure = 0;
					}

					// inside box
					cars[my_index].status = BOX;
					sprintf(pCommand, "[%d] changed status to BOX", cars[my_index].carNum);
					plog(pCommand);

					pthread_cond_signal(&in_box);
					pthread_cond_wait(&out_box, &tc_mutex);
					sprintf(pCommand, "[%d] left box", cars[my_index].carNum);
					dlog(pCommand);

					// left box
					tryBox = 0;
					cars[my_index].status = RUNNING;
					sprintf(pCommand, "[%d] changed status to RUNNING", cars[my_index].carNum);
					plog(pCommand);
				}

				pthread_mutex_unlock(&tc_mutex);
			}
		
			// if race was interrupted -> finish this car because he crossed lap
			if (shmem->status == OFF) {
				sem_wait(shmutex);
				addCarWIDs(my_index, 2);
				sem_post(shmutex);
				finish(my_index);
			}
		}

		// check fuel
		if (cars[my_index].fuel >= fuel4 && cars[my_index].fuel < fuel4+fuel1 && tryBox != 1) {
			// fuel for only 4 laps -> TRY TO GET INTO BOX
			sprintf(pCommand, "[%d] try box", cars[my_index].carNum);
			dlog(pCommand);
			tryBox = 1;
			
		} else if (cars[my_index].fuel >= fuel2 && cars[my_index].fuel < fuel2+fuel1 && cars[my_index].status != SAFETY) {
			// fuel for only 2 laps -> SAFETY MODE
			cars[my_index].status = SAFETY;
			sprintf(pCommand, "[%d] changed status to SAFETY", cars[my_index].carNum);
			plog(pCommand);
			tryBox = 1;
			
		} else if (cars[my_index].fuel <= 0) {
			// NO FUEL 
			cars[my_index].status = NO_FUEL;
			sprintf(pCommand, "[%d] changed status to NO FUEL", cars[my_index].carNum);
			plog(pCommand);

			sem_wait(shmutex);
			addCarWIDs(my_index, 1);
			sem_post(shmutex);
			
			finish(my_index);
		}

		usleep(1 * 1000 * 1000 * config.multiplier); // sleep 1 TIME UNIT
	}
    
    finish(my_index);
}
