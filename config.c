#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"


//remove all spaces from a string
void removeSpaces(char *str) {
    int count = 0; 

    for (int i = 0; str[i]; i++) 
        if (str[i] != ' ') 
            str[count++] = str[i]; 

    str[count] = '\0'; 
}

void checkCommas(char *str) {
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

    int nLines, ind = 0; 
    int values[8] = { 0 };

    //read confiFile
    while(fgets(lines[nLines], MAX_CHAR_LINE, configFile) != NULL && nLines < MAX_LINES) {
        nLines++;
    }
    
    //if nº of lines is different from MAX_LINES return -2 
    if (nLines != MAX_LINES) {
        fclose(configFile);
        return -2;
    }

    //convert values to int
    for (int j = 0; j< nLines; j++) {
        //printf("%d %d %d %d %d %d %d %d __\n",values[0], values[1],values[2], values[3], values[4], values[5], values[6], values[7]);
        if (j == 1 || j == 4) {
            removeSpaces(lines[j]);

            if (strchr(lines[j], ',') != NULL) {    //check from the separator ','
                //printf("#%s#\n", lines[j]);
                //checkCommas(lines[j]);
                //printf("_%s\n_", lines[j]);
                aux = strtok(lines[j], ",");
                //printf("!!%s!!\n", aux);
                values[ind] = atoi(aux);            //convert string to int
                ind++;

                aux = strtok(NULL, ",");
                //printf("!!!%s!!!\n", aux);
                values[ind] = atoi(aux);
                ind++;

            } else {
                ind += 2;
            }

        } else {
            values[ind] = atoi(lines[j]);
            ind++; 
        }
    }

    fclose(configFile);

    //return -1 in case the values are invalid
    if (values[3] < 3 || values[0] < 1 || values[1] < 1 || values[2] < 1 || values[4] < 1 || values[5] < 1 || values[6] < 1 || values[7] < 1) {
        return -1;
    }
    
    //set values in struct
    c->time = values[0];
    c->dist = values[1];
    c->nTurns = values[2];
    c->nTeams = values[3];
    c->tBreakDown = values[4];
    c->tMinBox = values[5];
    c->tMaxBox = values[6];
    c->fuelTank = values[7];

    return 0;
}
