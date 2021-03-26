/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#pragma once

#include "car.h"
#include "sharedmem.h"

#define MAX_TEAM_NAME 50
#define MAX_CARS 10

typedef struct team_t {
    char teamName[MAX_TEAM_NAME];
    struct car_t cars[MAX_CARS];


} team_t;

void team_manager_worker(shmem_t*, int);
