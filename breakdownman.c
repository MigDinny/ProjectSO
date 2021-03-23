#include <stdio.h>
#include "breakdownman.h"

void breakdown_manager_worker(config_t config, shmem_t *shmem, sem_t *mutex) {
    sleep(3);
    printf("test: %d", shmem->test);
}