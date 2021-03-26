/*

    Operating Systems - Final Project

    @author Miguel Dinis | 2019219010 | miguelbarroso@student.dei.uc.pt | github.com/MigDinny
    @author Rodrigo Ferreira | 2019220060 | rferreira@student.dei.uc.pt | github.com/IronMan988


*/

#pragma once


typedef struct car_t {
    int carNum;
    int speed;
    float consumption;
    float reliability;

} car_t;

void *car_worker();
