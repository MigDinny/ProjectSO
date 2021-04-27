/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/fcntl.h>

#include "include.h"

sem_t *logMutex;

FILE *logfile;

int init_log() {
    sem_unlink("LOG_MUTEX");
    logMutex = sem_open("LOG_MUTEX", O_CREAT | O_EXCL, 0700, 1);

    logfile = fopen("log.txt","w");

    if (logfile == NULL) {
        sem_unlink("LOG_MUTEX");
        return 1;
    }
    return 0;
}

void close_log() {

    fclose(logfile);

    sem_close(logMutex);
    sem_unlink("LOG_MUTEX");
}

void plog(char line[MAX_SIZE]) {
    

    char timestr[200];
    time_t timer;

    sem_wait(logMutex);     // wait to write on file
    
    timer = time(NULL);
    strcpy(timestr, ctime(&timer));
    
    printf("%.*s > %s\n", 8, timestr + 11, line);

    fprintf(logfile, "%.*s: %s\n", 8, timestr + 11, line);
    fflush(logfile);

    sem_post(logMutex);     // free mutex for next writer
}

void dlog(char line[MAX_SIZE]) {
    #ifdef DEBUG
        printf("DEBUG: %s\n", line);
    #endif
}

