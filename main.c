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
//#include "lib.h"


/*
    main function
    run as: ./formula1 [config-filename] 
*/
int main(int argc, char **argv) {

    if (argc != 2) {
		printf("Wrong number of parameters\n");
		exit(0);
	}




    FILE *configFile = fopen(argv[1], "r");
    

}