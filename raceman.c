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

// globals 
int pCommandsRead;
int nPipes = 0;
int totalCars = 0;
int finishedCars = 0;
int *processIDs;
int winner = 0;

fd_set read_set;
int *pipes;
int fdmax = 0;


void add_teams(char team[MAX_TEAM_NAME]) {      // create new team
    strcpy(teams[shmem->nTeams].teamName, team);
    teams[shmem->nTeams].nCars = 0;

    shmem->nTeams++;
}

int check_team(char team[MAX_TEAM_NAME]) {      // search for team, add it in case didn't found
                                                // returns -1 in case the shmem is full
    for (int i = 0; i < shmem->nTeams; i++) {
        if (strcmp(teams[i].teamName, team) == 0) {
            return i;
        }
    }
    if (shmem->nTeams < config.nTeams) {
        add_teams(team);
        return shmem->nTeams - 1;
    }
    return -1;
}

int add_car(char team[MAX_TEAM_NAME], int carNum, int speed, float consumption, int reliability){

    int teamID = check_team(team);

    if (teamID != -1) {
        if(teams[teamID].nCars < config.nCars) {
            int ind = teamID*config.nCars + teams[teamID].nCars;
            
            cars[ind].carNum = carNum;
            cars[ind].speed = speed;
            cars[ind].consumption = consumption;
            cars[ind].reliability = reliability;
		    cars[ind].fuel = config.fuelTank;
            cars[ind].boxStops = 0;
		    cars[ind].pos = 0;
		    cars[ind].laps = 0;
		    cars[ind].status = RUNNING;

            teams[teamID].nCars++;
            totalCars++;
            shmem->nCarsTotal++;

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

void start_race() {
    shmem->status = ON;
    
    int id[shmem->nTeams];
    int processID_temp[shmem->nTeams];
    int pipes_temp[shmem->nTeams + 1];

    for (int i = 0; i < shmem->nTeams; i++) {
        id[i] = i;
        shmem->runningCarsTotal += teams[i].nCars;

        pipe(channel);

        if((processID_temp[i] = fork()) == 0){
            close(channel[0]);
            team_manager_worker(id[i]);
            exit(0);
        }

        close(channel[1]);
        pipes_temp[i + 1] = channel[0];
    
        if (fdmax < pipes_temp[i + 1]) 
            fdmax = pipes_temp[i + 1];
    }

    nPipes = shmem->nTeams + 1;

    pipes_temp[0] = pipes[0];
    pipes = pipes_temp;

    processIDs = processID_temp;
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

        if (countWords == 11 && strcmp(address[0], "ADDCAR") == 0 && strcmp(address[1], "TEAM:") == 0 && 
                        strcmp(address[3], "CAR:") == 0 && strcmp(address[5], "SPEED:") == 0 && 
                        strcmp(address[7], "CONSUMPTION:") == 0 && strcmp(address[9], "RELIABILITY:") == 0 ) {

            int carNum = atoi(address[4]);
            int speed = atoi(address[6]);
            float consumption = atof(address[8]);
            int reliability = atoi(address[10]);
            
            if (carNum > 0 && speed > 0 && consumption > 0 && reliability >= 0 && reliability <= 100) {

                int error = add_car(address[2], carNum, speed, consumption, reliability);
                if (error == 0) {
                    sprintf(reply, "NEW CAR LOADED => %s", command);
                    plog(reply);
                }          

            } else {
                sprintf(reply, "WRONG COMMAND => %s", command);
                plog(reply);
            }

        } else if (countWords == 2 && strcmp(address[0], "START") == 0 && strcmp(address[1], "RACE!") == 0) {
            if (shmem->nTeams >= 3) {
                sprintf(reply, "NEW COMMAND RECEIVED: %s", command);

                start_race();                // TODO: function and verify current status

            } else if (shmem->nTeams < 3){             // check numbers of teams, at least 3 
                sprintf(reply, "CANNOT START, NOT ENOUGH TEAMS");

            } else {
                sprintf(reply, "COMMAND NOT ALLOWED, RACE ALREADY STARTED: %s", command);
            }
            plog(reply);

        }  else if (strcmp(command, "pSHMEM") == 0) {                // print shmem, remove for final version
            printf("!%d!\n", shmem->status);

            for (int i = 0; i < shmem->nTeams; i++) {
                for(int j = 0; j < teams[i].nCars; j++) {
                    printf("Team %d - [%s]; Car %d_%d\n", i, teams[i].teamName, j, cars[i*config.nCars + j].pos);
                }
            }

            for (int i = 0; i < 5; i++) {
                printf("%d_%d\n", shmem->carsWIDs[i], shmem->lastCarID);
            }

        } else {
            sprintf(reply, "WRONG COMMAND => %s", command);
            plog(reply);

        } 
    }
}

void end_race() {
    stats(0);

    shmem->status = OFF;

    // cleanup
    for (int i = 1; i <= shmem->nTeams; i++)
        close(pipes[i]);

    // kill child processes
    for (int u = 0; u < shmem->nTeams; u++)
        kill(processIDs[u], SIGTERM);
    
    // wait for all children
    for (int i = 0; i < shmem->nTeams; i++)
        wait(NULL);

    // reset pipes stuff to allow another race
    nPipes = 1;
    fdmax = pCommandsRead;
    int pipes_temp[1];
    pipes_temp[0] = pCommandsRead;
    finishedCars = 0;
    pipes = pipes_temp; 

    //reset shmem
    shmem->countBreakDowns = 0;
    shmem->countRefuels = 0;

    // reset cars positions
    for (int i = 0; i < shmem->nTeams; i++) {
        for(int j = 0; j < teams[i].nCars; j++) {
            cars[i*config.nCars + j].fuel = config.fuelTank;
            cars[i*config.nCars + j].laps = 0;
            cars[i*config.nCars + j].pos = 0;
            cars[i*config.nCars + j].status = RUNNING;
        }
        teams[i].status = FREE;
    }

    plog("RACE ENDED!");

    shmem->finishing = 0;

    // dont proceed if sigusr1 was caught and print stats
    if (!shmem->notSIGUSR1) {
        shmem->notSIGUSR1 = 1;
        return;
    }

    

    // close named pipe and terminate the program
    unlink(PIPE_COMMANDS);
    close(pCommandsRead);

    kill(getppid(), SIGTERM);
    exit(0);
}

void race_manager_worker() {
    
    char command_error[MAX_COMMAND];

    // open PIPE_COMMANDS as read only
    if ((pCommandsRead = open(PIPE_COMMANDS, O_RDONLY)) < 0) {
        sprintf(command_error, "ERROR opening pipe %s for writting CODE [%d]\n", PIPE_COMMANDS, errno);
        plog(command_error);
        exit(1);
    }

    // vars
    command_t cmd; 
    int pipe_commands[1]; 
    pipe_commands[0] = pCommandsRead;

    fdmax = pipe_commands[0]; // max file descriptor (it is always updated)
    pipes = pipe_commands; // update pointer
    nPipes = 1;
    shmem->nTeams = 0;

    // each iteration --> one set check
    while(1) {
        // reset set        
        FD_ZERO(&read_set);
        for (int i = 0; i < nPipes; i++) {
            FD_SET(pipes[i], &read_set);
        }

        // select the updated pipes, blocking until one comes
        if (select(fdmax + 1, &read_set, NULL, NULL, NULL) > 0 ) {
            // named pipe is triggered
            if(FD_ISSET(pipes[0], &read_set)) {
                read(pipes[0], &cmd, sizeof(command_t));
                check_input(cmd.command);
                cmd.command[0] = '\0';
            }

            // this only runs after start race because of nPipes | unnamed pipes triggered
            for (int i = 1; i < nPipes; i++) {

                if(FD_ISSET(pipes[i], &read_set)) {
                    read(pipes[i], &cmd, sizeof(cmd));
                    
                    if(cmd.carStatus == FINISHED) {
                        finishedCars++;
                        if (winner == 0 && cars[cmd.carID].laps >= config.nTurns) {
                            winner = 1;
                            char log_winner[MAX_COMMAND];
                            sprintf(log_winner, "CAR No %d WINS THE RACE", cars[cmd.carID].carNum);

                            plog(log_winner);
                        }
                    }
                    
                    cmd.carStatus = RUNNING;    // to avoid multiple checks above
                }
            }

            // if all cars finished, end race
            if (finishedCars == totalCars && totalCars != 0)
                end_race();
        }
    }

    exit(1);
}
