/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/
#include <semaphore.h>


// MAIN
#define PIPE_COMMANDS "/home/user/Desktop/np_main_raceman"
int bmpid;
int rmpid;
int gotSignal;

// RACEMAN
#define MAX_COMMAND 200


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
    int distance;
    int nTurns;
    int nTeams;
    int nCars;
    int tBreakDown;
    int tMinBox;
    int tMaxBox;
    float fuelTank;
	float multiplier;
} config_t;

typedef struct car_t {
	// from pipe - readonly
	int carNum;
    int speed;
    float consumption;
    int reliability;

	// control variables
	float fuel; // current fuel
	int pos; // current position
    int laps; // laps
	enum CAR_STATUS status;
} car_t;

typedef struct team_t {
    char teamName[MAX_TEAM_NAME];
    int nCars;
    enum BOX_STATUS status;
} team_t;

typedef struct shmem_t {
    enum RACE_STATUS status;
    int notSIGUSR1;
} shmem_t;

typedef struct command_t {
    int carID;
    enum CAR_STATUS carStatus;
    char command[MAX_COMMAND];
} command_t;


// GLOBALS

shmem_t *shmem; // shared memory struct POINTER
team_t *teams;  // pointing to teams in shmem
car_t *cars;    // pointing to cars in shmem

int shmid;
int pCommandsWrite;
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

int channel[2];     // unnamed pipe between raceman and teamman | 0 -> reading | 1 -> writing

int add_car(char [MAX_TEAM_NAME], int, int, float, int);

int check_team(char [MAX_TEAM_NAME]);

void add_teams(char [MAX_TEAM_NAME]);

void start_race();

void remove_commas(char*);

void check_input(char [MAX_COMMAND]);

void race_manager_worker();


// TEAM MANAGER
// below variables are cloned in each fork() of team-manager
void team_manager_worker(int);
int awaitingCars;
int awaitingSafetyCars;
int runningCars;
int boxCarIndex;
pthread_mutex_t tc_mutex;
pthread_cond_t in_box;
pthread_cond_t out_box;

// CAR THREAD

void *car_worker(void *);


// BREAKDOWN MANAGER

void breakdown_manager_worker();


// SHARED MEMORY

int init_shared_memory();

sem_t *init_shared_memory_mutex();

void clean_all_shared();

