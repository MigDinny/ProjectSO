/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <wait.h>

#include "raceman.h"
#include "teamman.h"
#include "sharedmem.h"


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

    for (int i = 0; i < shmem->config.nTeams; i++) wait(NULL);
    
}