#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int plog(char line[MAX_SIZE]) {

    printf("%s\n", line);
    fwrite(line, sizeof(char), strlen(line), logfile);
    fwrite("\n", sizeof(char), 2, logfile);

    return 0;
}

