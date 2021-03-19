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
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Include project libraries
*/
#include "config.h"
#include "sharedmem.h"

/*
    Constants
*/


/*
    Globals
*/
shmem_t *shmem; // shared memory struct POINTER
int shmid, sem;


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
		printf("No filename given\n");
		exit(3);
	}


    // init shared memory and semaphore to access shmem
    init_shared_memory(&shmid, &sem);


    // check config loading
    if ( (status = load_config(&(shmem->config), argv[1])) != 0) {
        printf("ERROR load_config CODE [%d]", status);
        exit(5);
    }

    exit(0);
}