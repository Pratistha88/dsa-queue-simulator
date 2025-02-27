#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> // For Sleep() on Windows

#include "generator.h"
#include "vehicle.h"


const char *lanes[] = {"AL2", "BL2", "CL2", "DL2"};

int main()
{
  
    srand(time(NULL));

   
    int vehicleIdCounter = 0;

    lane laneObjects[4];
    for (int i = 0; i < 4; i++)
    {
        initLane(&laneObjects[i], lanes[i]);
    }

    while (1)
    {

        int laneIndex = rand() % 4;
        printf("\nSelected lane: %s\n", lanes[laneIndex]);

        generateVehicle(&laneObjects[laneIndex], &vehicleIdCounter);

        char fileName[20]; 
        snprintf(fileName, sizeof(fileName), "./data/vehicles.txt");

        writeNewVehicleToFile(&laneObjects[laneIndex], "./data/vehicles.txt");

        printf("Vehicle %d generated in lane %s\n", vehicleIdCounter - 1, lanes[laneIndex]);

        Sleep(3500); // Sleep  (3.5 seconds)
    }

    return 0;
}
