/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/
#pragma once
#include <semaphore.h>


// MAIN
#define PIPE_COMMANDS "np_main_raceman"

// RACEMAN
#define MAX_COMMAND 100


//CONFIG
#define MAX_CHAR_LINE 20
#define MAX_LINES 7

//TEAM and CAR
#define MAX_TEAM_NAME 20

// ENUMS
enum BOX_STATUS {FREE, BUSY, RESERVED};
enum CAR_STATUS {RUNNING, SAFETY, BOX, NO_FUEL, FINISHED};
enum RACE_STATUS {ON, OFF};

// LOG
#define DEBUG
#define MAX_SIZE 50


// STRUCTS 

typedef struct config_t {
    int time;
    int dist;
    int nTurns;
    int nTeams;
    int nCars;
    int tBreakDown;
    int tMinBox;
    int tMaxBox;
    int fuelTank; 
} config_t;

typedef struct car_t {
    int carNum;
    int pos;
    int laps;
    enum CAR_STATUS;
    int speed;
    float consumption;
    int reliability;

} car_t;

typedef struct team_t {
    char teamName[MAX_TEAM_NAME];
    enum BOX_STATUS;
} team_t;

typedef struct shmem_t {

    enum RACE_STATUS;

} shmem_t;

typedef struct command{
    char command[MAX_COMMAND];
} command;


// GLOBALS

shmem_t *shmem; // shared memory struct POINTER
team_t *teams;  // pointing to teams in shmem
car_t *cars;    // pointing to cars in shmem

int shmid;
config_t config;
sem_t *shmutex;


// LOG

int init_log();

void close_log();

void plog(char [MAX_SIZE]);

void dlog(char [MAX_SIZE]);


// CONFIG

void remove_spaces(char *);

void check_commas(char *);

int load_config(char*);


// RACE MANAGER

void add_car(char [MAX_TEAM_NAME], int, int, float, int);

void start_race();

void remove_commas(char*);

void check_input(char [MAX_COMMAND]);

void race_manager_worker();


// TEAM MANAGER

void team_manager_worker(int);

// CAR MANAGER

void *car_worker(void *);

// BREAKDOWN MANAGER

void breakdown_manager_worker();


// SHARED MEMORY

int init_shared_memory();

sem_t *init_shared_memory_mutex();

void clean_all_shared();

