#include <unistd.h>

#include "car.h"


void *car_worker() {
    sleep(3);
    return NULL;
}
