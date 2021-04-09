/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "include.h"


void breakdown_manager_worker() {
    signal(SIGINT, SIG_DFL);

    plog("BREAKDOWN MANAGER STARTED");

    int delay = config.tBreakDown;
    float multiplier = 1/(config.time);

    while (1) {

        // wait for the race to start (use semaphore)

        // sleep until next calculation - delay is in seconds | * 1000 to convert to ms * 1000 to convert to us * multiplier 
        usleep(delay * 1000 * 1000 * multiplier);
        dlog("attempt to breakdown");
    }

}