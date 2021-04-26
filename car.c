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
	
	// minus one car 
	runningCars--;
	
	// [Unnamed Pipe] signal raceman: this car finished
	command_t send;
	send.carID = my_index;
	send.carStatus = FINISHED;
	write(channel[1], &send, sizeof(command_t));
	//close(channel[1]);
	
	// if last car, unblock team-manager (it is stuck on while loop)
	if (runningCars == 0)
		pthread_cond_signal(&in_box);
	
	pthread_exit(NULL);
}

void *car_worker(void *car_index) {
    int my_index = *((int*) car_index);
	int team_index = my_index / config.nCars;

	// minimum fuel for 1, 2 or 4 laps
	float fuel1 = cars[my_index].consumption * config.distance / cars[my_index].speed;
    float fuel2 = 2 * fuel1;
	float fuel4 = 4 * fuel1;
	int tryBox = 0; // 0 false 1 true

	printf("%f %f %f\n", fuel1, fuel2, fuel4);
	
	message_t receivedMSG;
	int receivedBytes = 0;
	int isCarFailure = 0;

	// each iteration is a TIME UNIT
	while(1) {

		// try to read a message from MQ; NO BLOCK
		receivedBytes = msgrcv(shmem->mqid, &receivedMSG, sizeof(message_t), my_index+1, IPC_NOWAIT);

		if (receivedBytes > 0) {
			printf("[%d] RECEIVED failure \n", my_index);
			cars[my_index].status = SAFETY;
			isCarFailure = 1;
			receivedBytes = 0;
			tryBox = 1;
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
				printf("[%d] finished\n", my_index);
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

					printf("[%d] in box\n", my_index);

					if (isCarFailure == 1) {
						isTeamCarFailure = 1;
						isCarFailure = 0;
					}

					// inside box
					cars[my_index].status = BOX;
					pthread_cond_signal(&in_box);
					pthread_cond_wait(&out_box, &tc_mutex);
					printf("[%d] left box\n", my_index);

					// left box
					tryBox = 0;
					awaitingCars--;
					if (cars[my_index].status == SAFETY)
						awaitingSafetyCars--;

					cars[my_index].status = RUNNING;
				}

				pthread_mutex_unlock(&tc_mutex);
			}
		
			// if race was interrupted -> finish this car because he crossed lap
			if (shmem->status == OFF)
				finish(my_index);
		}

		// check fuel
		if (cars[my_index].fuel >= fuel4 && cars[my_index].fuel < fuel4+fuel1 && tryBox != 1) {
			// fuel for only 4 laps -> TRY TO GET INTO BOX
			printf("[%d] try box\n", my_index);
			tryBox = 1;
			
		} else if (cars[my_index].fuel >= fuel2 && cars[my_index].fuel < fuel2+fuel1 && cars[my_index].status != SAFETY) {
			// fuel for only 2 laps -> SAFETY MODE
			cars[my_index].status = SAFETY;
			printf("[%d] safety mode\n", my_index);
			tryBox = 1;
			// @TODO some trigger here
		} else if (cars[my_index].fuel <= 0) {
			// NO FUEL 
			cars[my_index].status = NO_FUEL;
			printf("[%d] no fuel\n", my_index);
			finish(my_index);
		}

		//printf("[%d] m = %d  |  lap = %d  | fuel = %f\n", my_index, cars[my_index].pos, cars[my_index].laps, cars[my_index].fuel);
		usleep(1 * 1000 * 1000 * config.multiplier); // sleep 1 TIME UNIT
	}
    
    finish(my_index);
}
