//
// Created by Nokko on 2022-04-10.
// Simple arbitrary-datatype queue impl.
// Sort of janky, but works for a least-recently-used queue.
// TODO: Comments.
//

#include "queue.h"
#include "string.h"
queue *createQueue(queue *q, size_t max_size, size_t elem_size) {
    q->values = malloc(elem_size * max_size);
    q->elem_size = elem_size;
    q->size = max_size;
    q->nelem = 0;
    q->front = 0;
    q->rear = 0;
    return q;
}

void freeQueue(queue *q) {
    free(q->values);
    free(q);
}

void emptyQueue(queue *q) {
    memset(q->values, 0, q->size * q->elem_size);
}

bool isEmpty(queue *q) {
    return (q->nelem == 0);
}


bool isFull(queue *q) {
    return (q->nelem == q->size);
}

size_t enqueue(queue *q, void *data) {
    memcpy(((char*)q->values) + (q->rear * q->elem_size), data, q->elem_size);
    if (!isFull(q))
        q->nelem++;
    q->rear = (q->rear + 1) % q->size;
    if (q->rear >= q->size)
        q->rear = 0;
    return q->nelem;
}

size_t dequeue(queue *q, void *dest) {
    if (isEmpty(q))
        return q->nelem;
    memcpy(dest, ((char*)q->values) + (q->front * q->elem_size), q->elem_size);
    q->front = (q->front + 1) % q->size;
    return --q->nelem;
}
