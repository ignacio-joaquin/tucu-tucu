//
// Created by Nokko on 2022-04-10.
//

#ifndef RAY_MAZE_QUEUE_H
#define RAY_MAZE_QUEUE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define EMPTY_QUEUE INT_MIN
typedef struct Queue {
    void *values;
    size_t front;
    size_t rear;
    size_t nelem;
    size_t size;
    size_t elem_size;
} queue;

queue *createQueue(queue *q, size_t max_size, size_t elem_size);

bool isEmpty(queue *q);

bool isFull(queue *q);

size_t enqueue(queue *q, void *data);

size_t dequeue(queue *q, void *dest);

void freeQueue(queue *q);

void emptyQueue(queue *q);

#endif //RAY_MAZE_QUEUE_H
