/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988



*/

/*
    Include api libraries
*/
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>

/*
    Include project libraries
*/
#include "config.h"
#include "sharedmem.h"
#include "raceman.h"
#include "breakdownman.h"

/*
    Constants
*/


/*
    Globals
*/
shmem_t *shmem; // shared memory struct POINTER
int shmid;
sem_t *mutex; 

config_t config;

/*
    main function
    run as: ./formula1 [config-filename] 

        get parameters, loads configs and starts all proccesses

    exit codes:
        3 - absent param
        5 - problem loading config
*/
int main(int argc, char **argv) {

    int status = 0; // status codes for commands


    // check parameters
    if (argc != 2) {
		printf("ERROR No filename given\n");
		exit(3);
	}


    // init shared memory
    shmem = init_shared_memory(&shmid);
    if (shmid < 0) {
        printf("ERROR init_shared_memory CODE [%d]", shmid);
        exit(4);
    }


    // init semaphore to MUTEX shared memory
    mutex = init_shared_memory_mutex();


    // check config loading
    if ( (status = load_config(&config, argv[1])) != 0) {
        printf("ERROR load_config CODE [%d]", status);
        exit(5);
    }


    // create RACE MANAGER proccess
    if (fork() == 0) {
        // inside child, call worker
        race_manager_worker(config, shmem, mutex);
        exit(0);
    }

    // create BREAKDOWN MANAGER proccess
    if (fork() == 0) {
        // inside child, call worker
        breakdown_manager_worker(config, shmem, mutex);
        exit(0);
    }


    clean_all_shared(mutex, shmem, shmid);
    exit(0);
}