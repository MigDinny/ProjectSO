#pragma once

#include <semaphore.h>

#include "config.h"

typedef struct shmem_t {

    int test;

} shmem_t;


void *init_shared_memory(int *);

sem_t *init_shared_memory_mutex();

void clean_all_shared(sem_t *, shmem_t *, int);