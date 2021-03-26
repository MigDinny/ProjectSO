/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <semaphore.h>

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

typedef struct shmem_t {

    int test;
    sem_t *mutex; 
    config_t config;

} shmem_t;


// LOG

#define MAX_SIZE 50

int init_log();

int close_log();

int plog(char [MAX_SIZE]);


// CONFIG


#define MAX_CHAR_LINE 20
#define MAX_LINES 7

void remove_spaces(char *);

void check_commas(char *);

int load_config(config_t*, char*);


// RACE MANAGER

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

