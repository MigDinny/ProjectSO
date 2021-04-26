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
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/*
    Include project libraries
*/
#include "include.h"


void terminate(int k) {
    //stats();
    // print who won if exists

    plog("SIMULATOR CLOSING");

    if (k == 1)
        kill(rmpid, SIGKILL);

    // waits for RM >> kills bm >> waits for bm
    waitpid(rmpid, NULL, 0);
    kill(bmpid, SIGKILL);
    waitpid(bmpid, NULL, 0);

    // cleanup crew!
    close(pCommandsWrite);
    unlink(PIPE_COMMANDS);
    close_log();
    clean_all_shared(shmem, shmid);
    
    exit(0);
}

void sigint(int signum) {

    printf(" SIGINT detected\n");

    if (shmem->status == OFF)
        terminate(1);
    else
        shmem->status = OFF;

    terminate(0);
}

void sigtstp(int signum) {
    signal(SIGTSTP, sigtstp);
    
    plog("SIGTSTP detected!");
    // stats();
}

// the program ended normally, received this signal by raceman
void sigterm(int signum) {
    terminate(0);
}

// redirect signal to racemanager
void sigusr1_main(int signum) {
    plog("SIGUSR1 detected!");

    shmem->status = OFF;
    shmem->notSIGUSR1 = 0;
    gotSignal = 1;
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
    signal(SIGTSTP, SIG_IGN); // prevent this process from being suspended!
    signal(SIGUSR1, SIG_IGN); // prevent this process from dying!
    signal(SIGINT, SIG_IGN); // prevent this process from dying!

    gotSignal = 0;
    int status = 0; // status codes for commands
    init_log();

    // check parameters
    if (argc != 2) {
		printf("ERROR No filename given\n");
		exit(3);
	}


    // check config loading
    if ( (status = load_config(argv[1])) != 0) {
        printf("ERROR load_config CODE [%d]", status);
        exit(5);
    }
	
	
	 // init shared memory
    status = init_shared_memory();
    if (status < 0) {
        printf("ERROR init_shared_memory CODE [%d]", status);
        exit(4);
    }


    // init semaphore to MUTEX shared memory
    shmutex = init_shared_memory_mutex();


    // logging the first message
    plog("SIMULATOR STARTING");

    // create RACE MANAGER proccess
    if ((rmpid = fork()) == 0) {
        // inside child, call worker
        race_manager_worker(shmem);
        exit(0);
    }


    // create BREAKDOWN MANAGER proccess
    if ((bmpid = fork()) == 0) {
        // inside child, call worker
        breakdown_manager_worker(shmem);
        exit(0);
    }

    // initializing the SIGNAL redirections
    signal(SIGINT, sigint); // CTRL C
    signal(SIGTSTP, sigtstp); // CTRL Z
    signal(SIGTERM, sigterm); // SIGTERM BY raceman
    signal(SIGUSR1, sigusr1_main); // redirect sigusr1 to raceman

    // init named PIPE between main and race manager
    
    if ((mkfifo(PIPE_COMMANDS, O_CREAT|O_EXCL|0600) < 0) && (errno != EEXIST)) {
        printf("ERROR mkfifo CODE [%d]\n", errno);
        exit(7);
    }

    
    if ((pCommandsWrite = open(PIPE_COMMANDS, O_WRONLY)) < 0) {
        printf("ERROR opening pipe %s for writting CODE [%d]\n", PIPE_COMMANDS, errno);
        exit(8);
    }

    // loop receiving input to send to RACE MANAGER through PIPE
    
    char cmdSend[MAX_COMMAND];
    while(1) {
        fgets(cmdSend, MAX_COMMAND, stdin);     // reads the command and removes \n
        command_t cmd;                            // sends the command
        strcpy(cmd.command, cmdSend);
        write(pCommandsWrite, &cmd, sizeof(cmd));
    }

    /* DO NOT PUT ANY CODE BELOW HERE */

    // exited NOT through terminate() called by CTRL-C.
    // can we get even here? infinite loop before...
    exit(6);
}