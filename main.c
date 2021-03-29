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
#include "include.h"


/*
    Constants
*/


/*
    Globals
*/
shmem_t *shmem; // shared memory struct POINTER
int shmid;


void terminate() {
    plog("SIMULATOR CLOSING");

    close_log();
    clean_all_shared(shmem, shmid);
    exit(0);
}

void sigint(int signum) {

    printf(" SIGINT detected\n");

    //stats();

    terminate();
}

void sigtstp(int signum) {
    
    signal(SIGTSTP, sigtstp);

    printf(" SIGTSTP detected\n");

    // stats();
}


/*
    main function
    run as: ./formula1 [config-filename] 

        get parameters, loads configs and starts all proccesses

    exit codes:
        3 - absent param
        5 - problem loading config
*/
int main(int argc, char **argv) {

    // We need to ignore all signals first so the child processes inherit SIG_IGN.
    signal(SIGTSTP, SIG_IGN); // prevent this process to be suspended!
    signal(SIGUSR1, SIG_IGN); // prevent this process to die!
    signal(SIGINT, SIG_IGN); // prevent this process to die!

    int status = 0; // status codes for commands
    init_log();

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
    shmem->mutex = init_shared_memory_mutex();


    // check config loading
    if ( (status = load_config(&(shmem->config), argv[1])) != 0) {
        printf("ERROR load_config CODE [%d]", status);
        exit(5);
    }


    // create RACE MANAGER proccess
    if (fork() == 0) {
        // inside child, call worker
        race_manager_worker(shmem);
        exit(0);
    }


    // create BREAKDOWN MANAGER proccess
    if (fork() == 0) {
        // inside child, call worker
        breakdown_manager_worker(shmem);
        exit(0);
    }

    // logging the first message
    plog("SIMULATOR STARTING");
    
    // initializing the SIGNAL redirections
    signal(SIGINT, sigint); // CTRL C
    signal(SIGTSTP, sigtstp); // CTRL Z


    // loop receiving input to send to RACE MANAGER through PIPE
    char line[100];

    while (1) {

        fgets(line, 100, stdin);
        printf("%s", line);
    }


    // exited NOT through terminate() called by CTRL-C.
    // can we get even here? infinite loop before...
    exit(6);
}