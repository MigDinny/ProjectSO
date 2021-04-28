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
#include <sys/ipc.h>
#include <sys/msg.h>

/*
    Include project libraries
*/
#include "include.h"


void terminate(int k) {

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
    plog("SIGINT detected");
    

    if (shmem->status == OFF)
        terminate(1);
    else
        shmem->status = OFF;

    terminate(0);
}

void sigtstp(int signum) {
    plog("SIGTSTP detected!");
    stats(1);
}

// the program ended normally, received this signal by raceman
void sigterm(int signum) {
    terminate(0);
}

// redirect signal to racemanager
void sigusr1_main(int signum) {

    if (shmem->status == OFF) {
        plog("SIGUSR1 detected - race not started yet.");
        return;
    } else
        plog("SIGUSR1 detected!");
    
    shmem->finishing = 1;
    shmem->status = OFF;
    shmem->notSIGUSR1 = 0;
    shmem->status = OFF;

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

    int status = 0; // status codes for commands
    char command_error[MAX_COMMAND];
    init_log();

    // check parameters
    if (argc != 2) {
		plog("ERROR No filename given\n");
		exit(3);
	}


    // check config loading
    if ( (status = load_config(argv[1])) != 0) {
        sprintf(command_error, "ERROR load_config CODE [%d]", status);
        plog(command_error);
        exit(5);
    }
	
	
	 // init shared memory
    status = init_shared_memory();
    if (status < 0) {
        sprintf(command_error, "ERROR init_shared_memory CODE [%d]", status);
        plog(command_error);
        exit(4);
    }


    // init semaphore to MUTEX shared memory
    shmutex = init_shared_memory_mutex();


    // init message queues
    // and returns identifier
    shmem->mqid = msgget(IPC_PRIVATE, 0700 | IPC_CREAT);
    if (shmem->mqid == -1) {
        sprintf(command_error, "ERROR init_message_queue CODE [%d]", shmem->mqid);
        plog(command_error);
        exit(9);
    }


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
        sprintf(command_error, "ERROR mkfifo CODE [%d]\n", errno);
        plog(command_error);
        exit(7);
    }

    
    if ((pCommandsWrite = open(PIPE_COMMANDS, O_WRONLY)) < 0) {
        sprintf(command_error, "ERROR opening pipe %s for writting CODE [%d]\n", PIPE_COMMANDS, errno);
        plog(command_error);
        exit(8);
    }

    // loop receiving input to send to RACE MANAGER through PIPE
    
    char cmdSend[MAX_COMMAND];
    while(1) {
        fgets(cmdSend, MAX_COMMAND, stdin);     // reads the command and removes \n
        command_t cmd;                            // sends the command
        strcpy(cmd.command, cmdSend);

        if (shmem->status == ON || shmem->finishing == 1) {
            plog("COMMAND NOT ALLOWED, RACE ALREADY STARTED");
            cmd.command[0] = '\0';
            continue;
        }

        write(pCommandsWrite, &cmd, sizeof(cmd));
    }

    /* DO NOT PUT ANY CODE BELOW HERE */

    // exited NOT through terminate() called by CTRL-C.
    // can we get even here? infinite loop before...
    exit(6);
}