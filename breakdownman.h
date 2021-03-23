#pragma once

#include "config.h"
#include "sharedmem.h"

void breakdown_manager_worker(config_t, shmem_t*, sem_t*);