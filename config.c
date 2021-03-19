#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#define MAX_CHAR_LINE = 20

int load_config(config_t *c, char *filename) {

    FILE *configFile = fopen(filename, "r");
    char line[MAX_CHAR_LINE];

    while (fgets(line, 20, ))


    fclose(configFile);
}

