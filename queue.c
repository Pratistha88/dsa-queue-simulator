#include "queue.h"

void initQueue(Queue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

bool enqueue(Queue* q, Vehicle v) {
    if (q->count >= MAX_QUEUE) return false;
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->vehicles[q->rear] = v;
    q->count++;
    return true;
}

bool dequeue(Queue* q, Vehicle* v) {
    if (q->count <= 0) return false;
    *v = q->vehicles[q->front];
    q->front = (q->front + 1) % MAX_QUEUE;
    q->count--;
    return true;
}

int getCount(Queue* q) {
    return q->count;
}