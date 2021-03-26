#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "breakdownman.h"


void breakdown_manager_worker(shmem_t *shmem) {
    sleep(3);
    printf("test: %d", shmem->test);
}