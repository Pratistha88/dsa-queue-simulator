#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define ROAD_WIDTH 150
#define LANE_WIDTH 50

typedef struct {
    Queue laneA;
    Queue laneB;
    Queue laneC;
    Queue laneD;
    int currentLight;
    pthread_mutex_t mutex;
} TrafficSystem;

TrafficSystem system;

void drawRoads(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    
    SDL_Rect verticalRoad = {
        WINDOW_WIDTH/2 - ROAD_WIDTH/2, 
        0, 
        ROAD_WIDTH, 
        WINDOW_HEIGHT
    };
    SDL_RenderFillRect(renderer, &verticalRoad);

    SDL_Rect horizontalRoad = {
        0, 
        WINDOW_HEIGHT/2 - ROAD_WIDTH/2, 
        WINDOW_WIDTH, 
        ROAD_WIDTH
    };
    SDL_RenderFillRect(renderer, &horizontalRoad);
}

void* processVehicles(void* arg) {
    while (1) {
        FILE* file = fopen("vehicles.data", "r");
        if (!file) {
            sleep(1);
            continue;
        }

        char line[20];
        while (fgets(line, sizeof(line), file)) {
            Vehicle v;
            sscanf(line, "%[^:]:%c", v.vehicleNo, &v.lane);
            
            pthread_mutex_lock(&system.mutex);
            switch(v.lane) {
                case 'A': enqueue(&system.laneA, v); break;
                case 'B': enqueue(&system.laneB, v); break;
                case 'C': enqueue(&system.laneC, v); break;
                case 'D': enqueue(&system.laneD, v); break;
            }
            pthread_mutex_unlock(&system.mutex);
        }
        fclose(file);
        sleep(1);
    }
    return NULL;
}

int main() {
    // Initialize queues and mutex
    initQueue(&system.laneA);
    initQueue(&system.laneB);
    initQueue(&system.laneC);
    initQueue(&system.laneD);
    pthread_mutex_init(&system.mutex, NULL);

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Traffic Simulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED);

    // Create thread for processing vehicles
    pthread_t processThread;
    pthread_create(&processThread, NULL, processVehicles, NULL);

    // Main loop
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) 
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        drawRoads(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // ~60 FPS
    }

    // Cleanup
    pthread_mutex_destroy(&system.mutex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}