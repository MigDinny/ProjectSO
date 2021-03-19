#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"


int load_config(config_t *c, char *filename) {

    FILE *configFile = fopen(filename, "r");
    char line[MAX_CHAR_LINE];

    char *aux;

    int time, dist, nTurns, nTeams, tBreakDown, tMinBox, tMaxBox, fuelTank = 0; 

    fgets(line, MAX_CHAR_LINE, configFile);
    time = atoi(line);
 
    fgets(line, MAX_CHAR_LINE, configFile);
    aux = strtok(line, ", ");
    dist = atoi(aux);

    aux = strtok(NULL, ", ");
    nTurns = atoi(aux);

    fgets(line, MAX_CHAR_LINE, configFile); 
    nTeams = atoi(line);

    fgets(line, MAX_CHAR_LINE, configFile); 
    tBreakDown = atoi(line);

    fgets(line, MAX_CHAR_LINE, configFile); 
    aux = strtok(line, ", ");
    tMinBox = atoi(aux);

    aux = strtok(NULL, ", ");
    tMaxBox = atoi(aux);

    fgets(line, MAX_CHAR_LINE, configFile); 
    fuelTank = atoi(line);

    if (nTeams < 3 && time < 1 && dist < 1 && nTurns < 1 && tBreakDown < 1 && tMinBox < 1 && tMaxBox < 1 && fuelTank < 1) {

        return -1;
    }
   
    fclose(configFile);

    c->time = time;
    c->dist = dist;
    c->nTurns = nTurns;
    c->nTeams = nTeams;
    c->tBreakDown = tBreakDown;
    c->tMinBox = tMinBox;
    c->tMaxBox = tMaxBox;
    c->fuelTank = fuelTank;

    return 0;
}
