/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <semaphore.h>

// RACEMAN
#define MAX_COMMAND 100


//CONFIG
#define MAX_CHAR_LINE 20
#define MAX_LINES 7

//TEAM and CAR
#define MAX_TEAMS 10
#define BOX_STATUS_FREE 0
#define BOX_STATUS_BUSY 1
#define BOX_STATUS_RESERVED 2

#define MAX_TEAM_NAME 50
#define MAX_CARS 10
#define CAR_STATUS_RUNNING 0
#define CAR_STATUS_SAFETY 1
#define CAR_STATUS_BOX 2
#define CAR_STATUS_NO_FUEL 3
#define CAR_STATUS_FINISHED 4


// LOG
#define DEBUG 0
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
    int status;
    int speed;
    float consumption;
    float reliability;

} car_t;

typedef struct team_t {
    char teamName[MAX_TEAM_NAME];
    struct car_t cars[MAX_CARS];
    int box;

} team_t;


typedef struct shmem_t {
    team_t teams[MAX_TEAMS];

    int raceStatus;
    sem_t *mutex; 
    config_t config;

} shmem_t;




// LOG

int init_log();

int close_log();

int plog(char [MAX_SIZE]);

int dlog(char [MAX_SIZE]);


// CONFIG



void remove_spaces(char *);

void check_commas(char *);

int load_config(config_t*, char*);


// RACE MANAGER

void add_car();

void start_race();

void remove_commas(char *);

void race_manager_worker(shmem_t*);


// TEAM MANAGER

void team_manager_worker(shmem_t*, int);

// CAR MANAGER

void *car_worker();

// BREAKDOWN MANAGER

void breakdown_manager_worker(shmem_t*);


// SHARED MEMORY

void *init_shared_memory(int *);

sem_t *init_shared_memory_mutex();

void clean_all_shared(shmem_t *, int);

