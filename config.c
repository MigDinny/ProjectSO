#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int load_config(config_t *c, char *filename) {

    FILE *configFile = fopen(filename, "r");
    char line[MAX_CHAR_LINE];

    char *aux;

    int time, dist, nTurns, nTeams, tBreakDown, tMinBox, tMaxBox, fuelTank, i = 0; 

    fgets(line, MAX_CHAR_LINE, configFile);
    time = atoi(line);
 
    fgets(line, MAX_CHAR_LINE, configFile);
    aux = strtok(&line, ", ");
    dist = atoi(aux);

    aux = strtok(NULL, ", ");
    nTurns = atoi(aux);

    fgets(line, MAX_CHAR_LINE, configFile); 
    nTeams = atoi(line);

    fgets(line, MAX_CHAR_LINE, configFile); 
    tBreakDown = atoi(line);

    fgets(line, MAX_CHAR_LINE, configFile); 
    aux = strtok(&line, ", ");
    tMinBox = atoi(aux);

    aux = strtok(NULL, ", ");
    tMaxBox = atoi(aux);

    fgets(line, MAX_CHAR_LINE, configFile); 
    fuelTank = atoi(line);

    printf("%d %d %d %d %d %d %d %d\n", time, dist, nTurns, nTeams, tBreakDown, tMinBox, tMaxBox, fuelTank);
   
    fclose(configFile);
}
