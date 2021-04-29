/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

#include "include.h"

/*
    @TODO: #4 init shmem values!!! they probably must not be null or 0.
    first proccess to acquire lock to shared memory is kinda random which can bring some unexpected results
*/
int init_shared_memory() {

    shmid = shmget(IPC_PRIVATE, sizeof(shmem_t) + sizeof(team_t)*config.nTeams + sizeof(car_t)*config.nTeams*config.nCars , IPC_CREAT|0700);

	if (shmid < 0) return -1;
	
	shmem = (shmem_t*) shmat(shmid, NULL, 0);

    if (shmem == (shmem_t*) - 1) return -1;
	
	// init values
	
	shmem->status = OFF;
	shmem->nTeams = 0;
	shmem->notSIGUSR1 = 1;
    shmem->finishing = 0;
    shmem->runningCarsTotal = 0;

    for (int i = 0; i < 5; i++) {
        shmem->carsWIDs[i] = -1;
    }
    
	teams = (team_t*) (shmem+1);
	cars = (car_t*) (teams + config.nTeams);
	
	return 0;
}

sem_t *init_shared_memory_mutex() {

    sem_unlink("MUTEX");

    return sem_open("MUTEX", O_CREAT|O_EXCL,0700, 1);
}

void clean_all_shared() {

    sem_close(shmutex);
    sem_unlink("MUTEX");
    

    shmdt(shmem);
    shmctl(shmid, IPC_RMID, NULL);
}