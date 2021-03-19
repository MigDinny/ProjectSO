#include "sharedmem.h"

int init_shared_memory(int *shmid, int *sem) {

    *shmid = 5;
    *sem = 91;

    return 0;
}