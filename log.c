#include <stdio.h>

#include "log.h"

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

int log(char line[MAX_SIZE]) {
    
    fwrite(line, sizeof(char), sizeof(line), logfile);

    return 0;
}

