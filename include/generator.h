#ifndef GENERATOR_H
#define GENERATOR_H

#include "vehicle.h"

void generateVehicle(Lane *lane, int *vehicleIdCounter);
void writeNewVehicleToFile(Lane *lane, const char *fileName);

#endif // GENERATOR_H
