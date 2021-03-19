/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988



*/

/*
    Include api libraries
*/
#include <stdio.h>
#include <stdlib.h>

/*
    Include project libraries
*/
#include "config.h"

/*
    Constants
*/

#define MAX_CHAR_LINE_CONFIG 20


/*
    Globals
*/
// mutex
// shmem
config_t config; // config struct


/*
    main function
    run as: ./formula1 [config-filename] 

        get parameters, loads configs and starts all proccesses

    exit codes:
        3 - absent param
        5 - problem loading config
*/
int main(int argc, char **argv) {

    int status = 0; // status codes for commands


    // check parameters
    if (argc != 2) {
		printf("No filename given\n");
		exit(3);
	}



    // check config loading
    if ( (status = load_config(&config, argv[1])) != 0) {
        printf("ERROR CODE [%d]", status);
        exit(5);
    }
    printf("%d %d %d %d %d %d %d %d\n", config.time, config.dist, config.nTurns, config.nTeams, config.tBreakDown, config.tMinBox, config.tMaxBox, config.fuelTank);

    // init shared memory


}
