#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h> 


#include "raceman.h"
#include "teamman.h"


void race_manager_worker(config_t config, shmem_t *shmem, sem_t *mutex) {
        // TODO: proccess

    // creates TEAM_MANGERS
    for (int i = 0; i < config.nTeams; i++) {
        if(fork() == 0){
            team_manager_worker(config, shmem, mutex);
            exit(0);
        }
    }
    
}
