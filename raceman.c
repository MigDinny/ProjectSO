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


#include "include.h"


void remove_commas(char* str) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != ',');
    }
    *pw = '\0';
}

void add_car(){}

void start_race(){}

void receive_commands(){

    char command[MAX_COMMAND];
    char copy[MAX_COMMAND];
    char *address[MAX_COMMAND];
    
    while(1) {
        fgets(command, MAX_COMMAND, stdin);     // reads the command and removes \n
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

        if (strcmp(command, "EXIT") == 0) {
            plog(command);
            break;

        } else if (strcmp(address[0], "ADDCAR") == 0 && strcmp(address[1], "TEAM:") == 0 && 
                    strcmp(address[3], "CAR:") == 0 && strcmp(address[5], "SPEED:") == 0 && 
                    strcmp(address[7], "CONSUMPTION:") == 0 && strcmp(address[9], "RELIABILITY:") == 0 ) {

            char aux[MAX_COMMAND] = "NEW CAR LOADED => ";
            strcat(aux, command);
            plog(aux);

            add_car();                  // TODO: function

        } else if (strcmp(address[0], "START") == 0 && strcmp(address[1], "RACE") == 0) {
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


void race_manager_worker(shmem_t *shmem) {

    int id[shmem->config.nTeams];

    // creates TEAM_MANGERS
    for (int i = 0; i < shmem->config.nTeams; i++) {
        id[i] = i;

        if(fork() == 0){
            team_manager_worker(shmem, id[i]);
            exit(0);
        }
    }

    // needs to wait for all the program no initialize (breakdown manager) 
    //receive_commands();

    for (int i = 0; i < shmem->config.nTeams; i++) wait(NULL);
    
}
