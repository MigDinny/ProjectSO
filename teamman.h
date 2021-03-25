#pragma once

#include "config.h"
#include "sharedmem.h"


void team_manager_worker(config_t, shmem_t*, sem_t*);
