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

int pCommandsRead;
int nTeams = 0;


void add_teams(char team[MAX_TEAM_NAME]) {      // create new team
    strcpy(teams[nTeams].teamName, team);
    teams[nTeams].nCars = 0;

    nTeams++;
}

int check_team(char team[MAX_TEAM_NAME]) {      // search for team, add it in case didn't found
                                                // returns -1 in case the shmem is full
    for (int i = 0; i < nTeams; i++) {
        if (strcmp(teams[i].teamName, team) == 0) {
            return i;
        }
    }
    if (nTeams < config.nTeams) {
        add_teams(team);
        return nTeams - 1;
    }
    return -1;
}

int check_cars_nums() {
    return 10
}

int add_car(char team[MAX_TEAM_NAME], int carNum, int speed, float consumption, int reliability){

    int teamID = check_team(team);

    if (teamID != -1) {
        if(teams[teamID].nCars < config.nCars) {
            cars[teamID*config.nCars + teams[teamID].nCars].carNum = carNum;
            cars[teamID*config.nCars + teams[teamID].nCars].speed = speed;
            cars[teamID*config.nCars + teams[teamID].nCars].consumption = consumption;
            cars[teamID*config.nCars + teams[teamID].nCars].reliability = reliability;

            teams[teamID].nCars++;
        } else {
            plog("CAN'T ADD MORE CARS TO THIS TEAM");
            return -1;
        }
        
    } else {
        plog("CAN'T ADD MORE TEAMS");
        return -1;
    }
    return 0;
}

void start_race(){
    shmem->status = ON;     // mutex nedded
    
    int id[nTeams];

    for (int i = 0; i < nTeams; i++) {
        id[i] = i;

        if(fork() == 0){
            team_manager_worker(id[i]);
            exit(0);
        }
    }
}

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
    char reply[MAX_COMMAND];
        
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

        if (shmem->status == OFF && countWords == 11 && strcmp(address[0], "ADDCAR") == 0 && strcmp(address[1], "TEAM:") == 0 && 
                        strcmp(address[3], "CAR:") == 0 && strcmp(address[5], "SPEED:") == 0 && 
                        strcmp(address[7], "CONSUMPTION:") == 0 && strcmp(address[9], "RELIABILITY:") == 0 ) {

            int carNum = atoi(address[4]);
            int speed = atoi(address[6]);
            float consumption = atof(address[8]);
            int reliability = atoi(address[10]);
            
            if (carNum > 0 && speed > 0 && consumption > 0 && reliability > 0 && reliability < 100) {

                int error = add_car(address[2], carNum, speed, consumption, reliability);
                if (error == 0) {
                    sprintf(reply, "NEW CAR LOADED => %s", command);
                    plog(reply);
                }          

            } else {
                sprintf(reply, "WRONG COMMAND => %s", command);
                plog(reply);
            }

        } else if (shmem->status == OFF && countWords == 2 && strcmp(address[0], "START") == 0 && strcmp(address[1], "RACE") == 0) {
            if (nTeams >= 3) {
                sprintf(reply, "NEW COMMAND RECEIVED: %s", command);
                plog(reply);

                start_race();                // TODO: function and verify current status

            } else if (nTeams < 3){             // check numbers of teams, at least 3 
                sprintf(reply, "CANNOT START, NOT ENOUGH TEAMS");
                plog(reply);

            } else {
                sprintf(reply, "COMMAND NOT ALLOWED, RACE ALREADY STARTED: %s", command);
                plog(reply);
            }

        } else if (strcmp(command, "pSHMEM") == 0) {                // print shmem, remove for final version
            printf("!%d!\n", shmem->status);
            for (int i = 0; i < nTeams; i++) {
                for(int j = 0; j < teams[i].nCars; j++) {
                    printf("Team %d; Car %d_%d\n", i, j, cars[i*config.nCars + j].speed);
                }
            }

        } else if (shmem->status == OFF) {
            sprintf(reply, "WRONG COMMAND => %s", command);
            plog(reply);

        } else {
            sprintf(reply, "COMMAND NOT ALLOWED, RACE ALREADY STARTED: %s", command);
            plog(reply);
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
    
    // @TODO we need to close file descriptor pCommandsRead

    // opne PIPE_COMMANDS as read only
    if ((pCommandsRead = open(PIPE_COMMANDS, O_RDONLY)) < 0) {
        printf("ERROR opening pipe %s for writting CODE [%d]\n", PIPE_COMMANDS, errno);
        exit(1);
    }

    command cmd;
    while (1) {
        read(pCommandsRead, &cmd, sizeof(command));
        //printf("Received: %s", cmd.command);
        check_input(cmd.command);
    }
    

    for (int i = 0; i < nTeams; i++) wait(NULL);
    
}
