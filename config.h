#pragma once

#define MAX_CHAR_LINE 20
#define MAX_LINES 6

typedef struct config_t {
    int time;
    int dist;
    int nTurns;
    int nTeams;
    int tBreakDown;
    int tMinBox;
    int tMaxBox;
    int fuelTank; 
} config_t;


int load_config(config_t*, char*);
