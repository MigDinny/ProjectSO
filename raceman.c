#include "raceman.h"

void race_manager_worker(config_t config, shmem_t *shmem, sem_t *mutex) {

    shmem->test = 59;
}