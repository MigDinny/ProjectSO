#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h> 


#include "raceman.h"
#include "teamman.h"


void race_manager_worker(shmem_t *shmem) {
        // TODO: proccess

    // creates TEAM_MANGERS
    for (int i = 0; i < shmem->config.nTeams; i++) {
        if(fork() == 0){
            team_manager_worker(shmem);
            exit(0);
        }
    }

    wait(NULL);
    
}
