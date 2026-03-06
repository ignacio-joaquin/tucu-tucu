//
// Created by Nokko on 2022-04-09.
// Simple arbitrary-datatype stack impl.
// TODO: Comments.
//

#include <stdlib.h>
#ifndef RAY_MAZE_STACK_H
#define RAY_MAZE_STACK_H

typedef struct {
    size_t len; // # elements
    size_t cap; // capacity
    size_t element_size;
    void *data;
} arr_stack_t;

arr_stack_t *new_stack(size_t element_size, size_t initial_cap);

void free_stack(arr_stack_t *stack);

void realloc_stack(arr_stack_t *stack);

size_t push(arr_stack_t *stack, void *data);

size_t pop(arr_stack_t *stack, void *dest);

size_t peek(arr_stack_t *stack, void *dest);

#endif //RAY_MAZE_STACK_H
