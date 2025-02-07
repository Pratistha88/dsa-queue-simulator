#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h> 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 20
#define MAIN_FONT "/usr/share/fonts/TTF/DejaVuSans.ttf"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SCALE 1
#define ROAD_WIDTH 150
#define LANE_WIDTH 50
#define ARROW_SIZE 15

const char* VEHICLE_FILE = "vehicles.data";

typedef struct {
    int currentLight;
    int nextLight;
} SharedData;

// Vehicle structure
typedef struct Vehicle {
    int id;
    struct Vehicle* next;
} Vehicle;

// Queue structure for vehicles
typedef struct Queue {
    Vehicle* front;
    Vehicle* rear;
    int size;
} Queue;

// Function declarations
bool initializeSDL(SDL_Window **window, SDL_Renderer **renderer);
void drawRoadsAndLane(SDL_Renderer *renderer, TTF_Font *font);
void displayText(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y);
void drawLightForB(SDL_Renderer* renderer, bool isRed);
void refreshLight(SDL_Renderer *renderer, SharedData* sharedData);
void* chequeQueue(void* arg);
void* readAndParseFile(void* arg);
void enqueue(Queue* q, int id);
int dequeue(Queue* q);
void initQueue(Queue* q);
bool isQueueEmpty(Queue* q);
int queueSize(Queue* q);
void* vehicleQueueManagement(void* arg);

// Initialize Queue
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

// Enqueue a vehicle
void enqueue(Queue* q, int id) {
    Vehicle* newVehicle = (Vehicle*)malloc(sizeof(Vehicle));
    newVehicle->id = id;
    newVehicle->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newVehicle;
    } else {
        q->rear->next = newVehicle;
        q->rear = newVehicle;
    }
    q->size++;
}

// Dequeue a vehicle
int dequeue(Queue* q) {
    if (q->front == NULL) return -1;  // Queue is empty

    Vehicle* temp = q->front;
    int id = temp->id;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    q->size--;
    return id;
}

// Check if queue is empty
bool isQueueEmpty(Queue* q) {
    return q->size == 0;
}

// Get queue size
int queueSize(Queue* q) {
    return q->size;
}

int main() {
    pthread_t tQueue, tReadFile, tVehicleQueueManagement;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;    
    SDL_Event event;    

    if (!initializeSDL(&window, &renderer)) {
        return -1;
    }
    
    SDL_mutex* mutex = SDL_CreateMutex();
    SharedData sharedData = { 0, 0 }; // 0 => all red

    TTF_Font* font = TTF_OpenFont(MAIN_FONT, 24);
    if (!font) SDL_Log("Failed to load font: %s", TTF_GetError());

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    drawRoadsAndLane(renderer, font);
    SDL_RenderPresent(renderer);

    Queue vehicleQueues[4];
    for (int i = 0; i < 4; i++) {
        initQueue(&vehicleQueues[i]);
    }

    // Thread for processing queue and vehicle file reading
    pthread_create(&tQueue, NULL, chequeQueue, &sharedData);
    pthread_create(&tReadFile, NULL, readAndParseFile, vehicleQueues);
    pthread_create(&tVehicleQueueManagement, NULL, vehicleQueueManagement, vehicleQueues);

    bool running = true;
    while (running) {
        refreshLight(renderer, &sharedData);
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT) running = false;
    }
    SDL_DestroyMutex(mutex);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

bool initializeSDL(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }
    if (TTF_Init() < 0) {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Junction Diagram",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE,
                               SDL_WINDOW_SHOWN);
    if (!*window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(*renderer, SCALE, SCALE);

    if (!*renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

void drawLightForB(SDL_Renderer* renderer, bool isRed) {
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_Rect lightBox = {400, 300, 50, 30};
    SDL_RenderFillRect(renderer, &lightBox);

    if (isRed) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red light
    else SDL_SetRenderDrawColor(renderer, 11, 156, 50, 255);      // Green light

    SDL_Rect straight_Light = {405, 305, 20, 20};
    SDL_RenderFillRect(renderer, &straight_Light);
}

void refreshLight(SDL_Renderer *renderer, SharedData* sharedData) {
    if (sharedData->nextLight == sharedData->currentLight) return; // No change

    drawLightForB(renderer, sharedData->nextLight == 2);
    SDL_RenderPresent(renderer);

    printf("Light updated from %d to %d\n", sharedData->currentLight, sharedData->nextLight);
    sharedData->currentLight = sharedData->nextLight;
}

void* chequeQueue(void* arg) {
    SharedData* sharedData = (SharedData*)arg;
    while (1) {
        sharedData->nextLight = 0;  // Turn off lights
        sleep(5);
        sharedData->nextLight = 2;  // Turn on lights
        sleep(5);
    }
}

void* vehicleQueueManagement(void* arg) {
    Queue* vehicleQueues = (Queue*)arg;
    int lane;

    while (1) {
        for (lane = 0; lane < 4; lane++) {
            if (!isQueueEmpty(&vehicleQueues[lane])) {
                int vehicleId = dequeue(&vehicleQueues[lane]);
                printf("Processing vehicle %d in lane %d\n", vehicleId, lane);
                sleep(1);
            }
        }
        sleep(2);
    }
}

void* readAndParseFile(void* arg) {
    Queue* vehicleQueues = (Queue*)arg;

    while (1) {
        FILE* file = fopen(VEHICLE_FILE, "r");
        if (!file) {
            perror("Error opening file");
            continue;
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            char* vehicleNumber = strtok(line, ":");
            char* road = strtok(NULL, ":");

            if (vehicleNumber && road) {
                int lane = atoi(road); // Convert road string to lane index
                enqueue(&vehicleQueues[lane], atoi(vehicleNumber));
                printf("Enqueued Vehicle: %s in Lane: %s\n", vehicleNumber, road);
            }
        }

        fclose(file);
        sleep(2);
    }
}

void displayText(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y) {
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {x, y, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
}

void drawRoadsAndLane(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 211,211,211,255);

    SDL_Rect verticalRoad = {WINDOW_WIDTH / 2 - ROAD_WIDTH / 2, 0, ROAD_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &verticalRoad);

    SDL_Rect horizontalRoad = {0, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2, WINDOW_WIDTH, ROAD_WIDTH};
    SDL_RenderFillRect(renderer, &horizontalRoad);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i <= 3; i++) {
        SDL_RenderDrawLine(renderer, 
            0, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i,
            WINDOW_WIDTH / 2 - ROAD_WIDTH / 2, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i
        );
        SDL_RenderDrawLine(renderer, 
            800, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i,
            WINDOW_WIDTH / 2 + ROAD_WIDTH / 2, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i
        );
        SDL_RenderDrawLine(renderer,
            WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i, 0,
            WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2
        );
        SDL_RenderDrawLine(renderer,
            WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i, 800,
            WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 + LANE_WIDTH * i, WINDOW_HEIGHT / 2 + ROAD_WIDTH / 2
        );
    }
    displayText(renderer, font, "A", 400, 10);
    displayText(renderer, font, "B", 400, 770);
    displayText(renderer, font, "D", 10, 400);
    displayText(renderer, font, "C", 770, 400);
}
