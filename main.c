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

void remove_commas(char* str) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != ',');
    }
    *pw = '\0';
}

void receive_commands(){

    char command[MAX_COMMAND];
    char copy[MAX_COMMAND];
    char *address[MAX_COMMAND];
    
    while(1) {
        fgets(command, MAX_COMMAND, stdin);     // reads the command and removes \n
        
        if (strlen(command) > 1) {
            command[strcspn(command, "\n")] = 0;
        
            strcpy(copy, command);              // copy intial command to parts indice 0

            char *ind = strtok(copy, " ");               // splits string in parts
            address[0] = ind;

            int count_words = 0;
            while(ind != NULL && count_words < MAX_COMMAND) {
                ind = strtok(NULL, " ");
                address[count_words +1] = ind;
                count_words ++;
            }

            for (int i = 0; i < count_words; i++) {           // copy each part of the string to parts, removing , if in the right format
                remove_commas(address[i]);
            }
            dlog("here");
            if (count_words == 10 && strcmp(address[0], "ADDCAR") == 0 && strcmp(address[1], "TEAM:") == 0 && 
                            strcmp(address[3], "CAR:") == 0 && strcmp(address[5], "SPEED:") == 0 && 
                            strcmp(address[7], "CONSUMPTION:") == 0 && strcmp(address[9], "RELIABILITY:") == 0 ) {
                dlog("here2");
                int carNum = atoi(address[4]);
                int speed = atoi(address[6]);
                float consumption = atof(address[8]);
                int reliability = atoi(address[10]);

                if (carNum > 0 && speed > 0 && consumption > 0 && reliability > 0 && reliability < 100) {
                    char aux[MAX_COMMAND] = "NEW CAR LOADED => ";
                    strcat(aux, command);
                    plog(aux);

                    add_car();                  // TODO: function
                } else {
                    char aux[MAX_COMMAND] = "WRONG COMMAND => ";
                    strcat(aux, command);
                    plog(aux);
                }

            } else if (count_words == 2 && strcmp(address[0], "START") == 0 && strcmp(address[1], "RACE") == 0) {
                char aux[MAX_COMMAND] = "NEW COMMAND RECEIVED: ";
                strcat(aux, command);
                plog(aux);

                start_race();                // TODO: function and verify current status

            } else {
                char aux[MAX_COMMAND] = "WRONG COMMAND => ";
                strcat(aux, command);
                plog(aux);
            }
        }
    }

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

    

    // logging the first message
    plog("SIMULATOR STARTING");

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

    // initializing the SIGNAL redirections
    signal(SIGINT, sigint); // CTRL C
    signal(SIGTSTP, sigtstp); // CTRL Z


    // loop receiving input to send to RACE MANAGER through PIPE
    receive_commands();


    // exited NOT through terminate() called by CTRL-C.
    // can we get even here? infinite loop before...
    exit(6);
}