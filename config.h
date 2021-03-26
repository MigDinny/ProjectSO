/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#pragma once

#define MAX_CHAR_LINE 20
#define MAX_LINES 7

typedef struct config_t {
    int time;
    int dist;
    int nTurns;
    int nTeams;
    int nCars;
    int tBreakDown;
    int tMinBox;
    int tMaxBox;
    int fuelTank; 
} config_t;

void remove_spaces(char *);

void check_commas(char *);

int load_config(config_t*, char*);
