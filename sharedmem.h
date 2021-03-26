/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#pragma once

#include <semaphore.h>

#include "config.h"

typedef struct shmem_t {

    int test;
    sem_t *mutex; 
    config_t config;

} shmem_t;


void *init_shared_memory(int *);

sem_t *init_shared_memory_mutex();

void clean_all_shared(shmem_t *, int);