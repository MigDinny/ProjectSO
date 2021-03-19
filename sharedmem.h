#pragma once

#include "config.h"

typedef struct shmem_t {

    config_t config;

} shmem_t;


int init_shared_memory(int *, int *);