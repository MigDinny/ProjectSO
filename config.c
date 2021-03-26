/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"


// remove all spaces from a string
void remove_spaces(char *str) {
    int count = 0; 

    for (int i = 0; str[i]; i++) 
        if (str[i] != ' ') 
            str[count++] = str[i]; 

    str[count] = '\0'; 
}

void check_commas(char *str) {
    int count = 0;
    int first = 0; 

    for (int i = 0; str[i]; i++) 
        if (str[i] != ',') {
            str[count++] = str[i]; 
        } else if (str[i] == ',' && first == 0) {
            first = 1;
            str[count++] = str[i];
        }

    str[count] = '\0'; 
}

int load_config(config_t *c, char *filename) {

    FILE *configFile = fopen(filename, "r");
    
    char lines[MAX_LINES][MAX_CHAR_LINE];
    char *aux;

    int nLines = 0, ind = 0; 
    int values[9] = { 0 };

    // read confiFile
    while(fgets(lines[nLines], MAX_CHAR_LINE, configFile) != NULL && nLines < MAX_LINES) {
        nLines++;
    }

    fclose(configFile);
    
    // if no of lines is different from MAX_LINES return -2 
    if (nLines != MAX_LINES) {
        return 2;
    }

    // convert values to int
    for (int j = 0; j < nLines; j++) {
        if (j == 1 || j == 5) {
            remove_spaces(lines[j]);

            if (strchr(lines[j], ',') != NULL) {    // check separator ','
                check_commas(lines[j]);
                aux = strtok(lines[j], ",");

                if (aux != NULL) {
                    values[ind] = atoi(aux);         
                }            
                ind++;
                
                aux = strtok(NULL, ",");

                if (aux != NULL) {
                    values[ind] = atoi(aux);
                }
                ind++;

            } else {
                ind += 2;
            }

        } else {
            values[ind] = atoi(lines[j]);           // convert string to int
            ind++; 
        }
    }
    
    // return -1 in case the values are invalid
    if (values[3] < 3 || values[0] < 1 || values[1] < 1 || values[2] < 1 || values[4] < 1 || values[5] < 1 || values[6] < 1 || values[7] < 1 || values[8] < 1 ) {
        return 1;
    }

    // TODO: debug next lines, config NULL
    // set values in struct
    c->time = values[0];
    c->dist = values[1];
    c->nTurns = values[2];
    c->nTeams = values[3];
    c->nCars = values[4];
    c->tBreakDown = values[5];
    c->tMinBox = values[6];
    c->tMaxBox = values[7];
    c->fuelTank = values[8];

    return 0;
}
