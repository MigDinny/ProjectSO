/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include.h"

FILE *logfile;

int init_log() {
    logfile = fopen("log.txt","w");

    if (logfile == NULL) {
        return 1;
    }
    return 0;
}

int close_log() {

    fclose(logfile);

    return 0;
}

int plog(char line[MAX_SIZE]) {
    char timestr[10];

    time_t timer;
    struct tm* tm_info;
    timer = time(NULL);
    tm_info = localtime(&timer);
    strftime(timestr, 10, "%H:%M:%S ", tm_info);

    printf("%s%s\n", timestr, line);
    fwrite(timestr, sizeof(char),strlen(timestr), logfile);
    fwrite(line, sizeof(char), strlen(line), logfile);
    fwrite("\n", sizeof(char), 2, logfile);

    return 0;
}

