#include "sharedmem.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

/*
    @TODO: #4 init shmem values!!! they probably must not be null or 0.
    first proccess to acquire lock to shared memory is kinda random which can bring some unexpected results
*/
void *init_shared_memory(int *shmid) {

    *shmid = shmget(IPC_PRIVATE, sizeof(shmem_t), IPC_CREAT|0666);

    return shmat(*shmid, NULL, 0);
}

sem_t *init_shared_memory_mutex() {

    sem_unlink("MUTEX");

    return sem_open("MUTEX", O_CREAT|O_EXCL,0700, 1);
}

void clean_all_shared( shmem_t *shmem, int shmid) {

    sem_close(shmem->mutex);
    sem_unlink("MUTEX");
    

    shmdt(shmem);
    shmctl(shmid, IPC_RMID, NULL);
}