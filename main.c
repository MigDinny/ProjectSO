#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main(int argc, char **argv) {

    for (int i = 0; i < 3; i++)
        printf("%d\n", i);
    
    printf("HELLO WORLD");
}