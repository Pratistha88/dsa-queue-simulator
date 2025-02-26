#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define MAX_QUEUE 100
#define PRIORITY_THRESHOLD 10
#define NORMAL_THRESHOLD 5

typedef struct {
    char vehicleNo[9];
    char lane;
} Vehicle;

typedef struct {
    Vehicle vehicles[MAX_QUEUE];
    int front;
    int rear;
    int count;
} Queue;

void initQueue(Queue* q);
bool enqueue(Queue* q, Vehicle v);
bool dequeue(Queue* q, Vehicle* v);
int getCount(Queue* q);

#endif