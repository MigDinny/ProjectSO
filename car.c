/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


#include "include.h"


void *car_worker(void *car_index) {
    int my_index = *((int*) car_index);

	// <<< DEBUG - this must be set when ADD CAR is received >>>
		cars[my_index].carNum = my_index;
		cars[my_index].speed = 20;
		cars[my_index].consumption = 3;
		cars[my_index].reliability = 0.90;
		cars[my_index].fuel = config.fuelTank;
		cars[my_index].pos = 0;
		cars[my_index].laps = 0;
		cars[my_index].status = RUNNING;
	// <<< DEBUG >>>

	// minimum fuel for 1, 2 or 4 laps
	float fuel1 = cars[my_index].consumption * config.distance / cars[my_index].speed;
    float fuel2 = 2 * fuel1;
	float fuel4 = 4 * fuel1;
	int tryBox = 0; // 0 false 1 true

	printf("%f %f %f\n", fuel1, fuel2, fuel4);

	sleep(2);

	// car started race
    printf("car started [%d]\n", my_index);
	
	// each iteration is a TIME UNIT
	while(1) {

		// @TODO ACQUIRE MUTEXES and COND_VARS
		// Wait for START RACE


		// proccess a time unit
		if (cars[my_index].status == SAFETY) {
			cars[my_index].pos += 0.3*cars[my_index].speed;
			cars[my_index].fuel -= 0.4*cars[my_index].consumption;
		} else {
			cars[my_index].pos += cars[my_index].speed;
			cars[my_index].fuel -= cars[my_index].consumption;
		}
		

		// new lap
		if (cars[my_index].pos >= config.distance) {
			cars[my_index].pos -= config.distance;
			cars[my_index].laps++;

			if (cars[my_index].laps >= config.nTurns) {
				// finished race
				cars[my_index].status = FINISHED;
				printf("[%d] finished\n", my_index);
				pthread_exit(NULL);
				// @TODO some trigger here
			}
		}

		// check fuel
		if (cars[my_index].fuel >= fuel4 && cars[my_index].fuel <  fuel4+fuel1 && tryBox != 1) {
			// fuel for only 4 laps -> TRY TO GET INTO BOX
			printf("[%d] try box\n", my_index);
			tryBox = 1;

			// @TODO some trigger here
		} else if (cars[my_index].fuel >= fuel2 && cars[my_index].fuel < fuel2+fuel1 && cars[my_index].status != SAFETY) {
			// fuel for only 2 laps -> SAFETY MODE
			cars[my_index].status = SAFETY;
			printf("[%d] safety mode\n", my_index);

			// @TODO some trigger here
		} else if (cars[my_index].fuel <= 0) {
			// NO FUEL 
			cars[my_index].status = NO_FUEL;
			
			printf("[%d] no fuel\n", my_index);
			// @TODO some trigger here
			pthread_exit(NULL);
		}

		

		
		printf("[%d] m = %d  |  lap = %d  | fuel = %f\n", my_index, cars[my_index].pos, cars[my_index].laps, cars[my_index].fuel);
		usleep(1 * 1000 * 1000 * config.multiplier); // sleep 1 TIME UNIT
	}
    
    
	
    pthread_exit(NULL);
    return NULL;
}
