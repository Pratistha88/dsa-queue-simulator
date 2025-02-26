#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "queue.h"

void generateVehicleNumber(char* buffer) {
    buffer[0] = 'A' + rand() % 26;
    buffer[1] = 'A' + rand() % 26;
    buffer[2] = '0' + rand() % 10;
    buffer[3] = 'A' + rand() % 26;
    buffer[4] = 'A' + rand() % 26;
    buffer[5] = '0' + rand() % 10;
    buffer[6] = '0' + rand() % 10;
    buffer[7] = '0' + rand() % 10;
    buffer[8] = '\0';
}

char generateLane() {
    char lanes[] = {'A', 'B', 'C', 'D'};
    return lanes[rand() % 4];
}

int main() {
    FILE* file = fopen("vehicles.data", "w");
    if (!file) return 1;

    srand(time(NULL));

    while (1) {
        Vehicle v;
        generateVehicleNumber(v.vehicleNo);
        v.lane = generateLane();
        fprintf(file, "%s:%c\n", v.vehicleNo, v.lane);
        fflush(file);
        sleep(1);
    }

    return 0;
}
