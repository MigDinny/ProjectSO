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

char intInArray(int array[], int length, int num) {
    // check the array for the number given
    for (int i = 0; i < length; i++) {
        if (array[i] == num) 
            return 1;
    }
    return 0;
}


int find_first (int ignoreCarIDs [], int length) {
    int carID = -1;

    // gets the car in 1º position ignoring the carsIDs given in the array
    for (int t = 0; t < shmem->nTeams; t++) {
        for (int c = 0; c < teams[t].nCars; c++) {
            if (!intInArray(ignoreCarIDs, length, t*config.nCars + c)){

                if (carID == -1) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps > cars[carID].laps) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos > cars[carID].pos) {
                    carID = t*config.nCars + c;
                }
            }
        }
    }

    return carID;
}


int find_last (int ignoreCarIDs [], int length) {
    int carID = -1;

    // gets the car in last position ignoring the carsIDs given in the array
    for (int t = 0; t < shmem->nTeams; t++) {
        for (int c = 0; c < teams[t].nCars; c++) {
            if (!intInArray(ignoreCarIDs, length, t*config.nCars + c)){

                if (carID == -1) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps < cars[carID].laps) {
                    carID = t*config.nCars + c;

                } else if (cars[t*config.nCars + c].laps == cars[carID].laps && cars[t*config.nCars + c].pos < cars[carID].pos) {
                    carID = t*config.nCars + c;
                }
            }
        }
    }

    return carID;
}

void stats () {
    char statsRace[MAX_COMMAND];
    plog("STATS:");
    
    int carsIDs [5] = {-1, -1, -1, -1, -1};
    
    // print 5 first car
    int loop;
    if (shmem->nCarsTotal > 5) {
        loop = 5;
    } else {
        loop = shmem->nCarsTotal;
    }

    for (int i = 0; i < loop; i++) {
        carsIDs[i] = find_first(carsIDs, sizeof(carsIDs) / sizeof(int));
    }

    for (int i = 0; i < loop; i++) {
        sprintf(statsRace, "%d position: Car: %d; Team: %s; Laps: %d; Box stops: %d", i + 1, cars[carsIDs[i]].carNum, cars[carsIDs[i]].team, cars[carsIDs[i]].laps, cars[carsIDs[i]].boxStops);
        plog(statsRace);
    }

    // print last car
    if (shmem->nCarsTotal >= 6) {
        int carsIDsL [0];
        int lastID = find_last(carsIDsL, 0);

        plog("...");
        sprintf(statsRace, "%d position: Car: %d; Team: %s; Laps: %d; Box stops: %d", shmem->nCarsTotal, cars[lastID].carNum, cars[lastID].team, cars[lastID].laps, cars[lastID].boxStops);
        plog(statsRace);
    }
    

    // print info of each team
    for (int i = 0; i < shmem->nTeams; i++) {
        sprintf(statsRace, "%s:\n\tNumber of refills: %d\n\tNumber of breakdowns: %d\n", teams[i].teamName, teams[i].countRefuels, teams[i].countBreakDowns);
        plog(statsRace);
    }

    sprintf(statsRace, "Current number of cars in lane: %d\n", runningCars);
    plog(statsRace); 

}

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


    // init message queues
    // and returns identifier
    shmem->mqid = msgget(IPC_PRIVATE, 0700 | IPC_CREAT);
    if (shmem->mqid == -1) {
        printf("ERROR init_message_queue CODE [%d]", shmem->mqid);
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

        if (shmem->status == ON) {
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