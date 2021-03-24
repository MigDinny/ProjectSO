#include "sharedmem.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

void *init_shared_memory(int *shmid) {

    *shmid = shmget(IPC_PRIVATE, sizeof(shmem_t), IPC_CREAT|0666);

    return shmat(*shmid, NULL, 0);
}

sem_t *init_shared_memory_mutex() {

    sem_unlink("MUTEX");

    return sem_open("MUTEX", O_CREAT|O_EXCL,0700, 1);
}

void clean_all_shared(sem_t *mutex, shmem_t *shmem, int shmid) {

    sem_close(mutex);
    sem_unlink("MUTEX");
    

    shmdt(shmem);
    shmctl(shmid, IPC_RMID, NULL);
}