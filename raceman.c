/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include "include.h"



void add_car(char team[MAX_TEAM_NAME], int carNum, int speed, float consumption, int reliability){
    printf("car added: %s, %d, %d, %f, %d\n", team, carNum, speed, consumption, reliability);
}

void start_race(){}

void remove_commas(char* str) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != ',');
    }
    *pw = '\0';
}

void check_input(char command[MAX_COMMAND]){

    char copy[MAX_COMMAND];
    char *address[MAX_COMMAND];
        
    if (strlen(command) > 1) {
        command[strcspn(command, "\n")] = 0;
    
        strcpy(copy, command);              // copy intial command to parts indice 0

        char *ind = strtok(copy, " ");               // splits string in parts
        address[0] = ind;

        int countWords = 0;
        while(ind != NULL && countWords < MAX_COMMAND) {
            ind = strtok(NULL, " ");
            address[countWords +1] = ind;
            countWords ++;
        }

        for (int i = 0; i < countWords; i++) {           // copy each part of the string to parts, removing , if in the right format
            remove_commas(address[i]);
        }

        if (countWords == 11 && strcmp(address[0], "ADDCAR") == 0 && strcmp(address[1], "TEAM:") == 0 && 
                        strcmp(address[3], "CAR:") == 0 && strcmp(address[5], "SPEED:") == 0 && 
                        strcmp(address[7], "CONSUMPTION:") == 0 && strcmp(address[9], "RELIABILITY:") == 0 ) {

            int carNum = atoi(address[4]);
            int speed = atoi(address[6]);
            float consumption = atof(address[8]);
            int reliability = atoi(address[10]);

            if (carNum > 0 && speed > 0 && consumption > 0 && reliability > 0 && reliability < 100) {
                char aux[MAX_COMMAND] = "NEW CAR LOADED => ";
                strcat(aux, command);
                plog(aux);

                add_car(address[2], carNum, speed, consumption, reliability);                  // TODO: function
            } else {
                char aux[MAX_COMMAND] = "WRONG COMMAND => ";
                strcat(aux, command);
                plog(aux);
            }

        } else if (countWords == 2 && strcmp(address[0], "START") == 0 && strcmp(address[1], "RACE") == 0) {
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

void sigusr1(int signum) {

    plog("SIGUSR1 received, race interrupted!");

    // stats();
}

void race_manager_worker() {

    signal(SIGUSR1, sigusr1); // interrupt race!
    signal(SIGINT, SIG_DFL);
    
    // opne PIPE_COMMANDS as read only
    
    int pCommands;
    if ((pCommands = open(PIPE_COMMANDS, O_RDONLY)) < 0) {
        printf("ERROR opening pipe %s for writting CODE [%d]\n", PIPE_COMMANDS, errno);
        exit(1);
    }

    command cmd;
    while (1) {
        read(pCommands, &cmd, sizeof(command));
        printf("Received: %s", cmd.command);
        check_input(cmd.command);
    }
    

    //int id[config.nTeams];

    // creates TEAM_MANGERS
    /*
    for (int i = 0; i < config.nTeams; i++) {
        id[i] = i;

        if(fork() == 0){
            team_manager_worker(id[i]);
            exit(0);
        }
    }

    for (int i = 0; i < config.nTeams; i++) wait(NULL);*/
    
}
