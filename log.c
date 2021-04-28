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

void plog(char line[300], ...) {
    
    char timestr[20];
    char forward[300];
    forward[0] = '\0';
    time_t timer;

    sem_wait(logMutex);     // wait to write on file
    
    timer = time(NULL);
    strcpy(timestr, ctime(&timer));
    
    sprintf(forward, "%.*s > ", 8, timestr + 11);
    strcat(forward, line);
    strcat(forward, "\n");
    
    va_list args;
    va_list args2;
    va_start(args, line);
    va_start(args2, line);

    vprintf(forward, args);

    vfprintf(logfile, forward, args2);

    fflush(logfile);

    sem_post(logMutex);     // free mutex for next writer
}

void dlog(char line[300], ...) {
    #ifdef DEBUG
        char forward[300];
        forward[0] = '\0';

        sprintf(forward, "[DEBUG] >> ");
        strcat(forward, line);
        strcat(forward, "\n");

        va_list args;
        va_start(args, line);

        vprintf(forward, args);
    
    #endif
}

