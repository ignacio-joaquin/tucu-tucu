//
// Created by Nokko on 2022-04-09.
//

#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t len; // # elements
    size_t cap; // capacity
    size_t element_size;
    void *data;
} arr_stack_t;

arr_stack_t *new_stack(size_t element_size, size_t initial_cap) {
    arr_stack_t *stack = (arr_stack_t *) calloc(1,sizeof (arr_stack_t));
    if (stack == NULL) return NULL;
    stack->data = malloc(element_size * initial_cap);
    if (stack->data == NULL) {
        free(stack);
        return NULL;
    }
    stack->element_size = element_size;
    stack->cap = initial_cap;
    return stack;
}

void free_stack(arr_stack_t *stack) {
    free(stack->data);
    free(stack);
}

void realloc_stack(arr_stack_t *stack) {
    size_t new_cap = (stack->element_size * stack->len) * 2;
    void *new_data = realloc(stack->data, new_cap);
    stack->data = new_data;
    stack->cap = new_cap;
}

size_t push(arr_stack_t *stack, void *data) {
    stack->len += 1;
    if ((stack->len * stack->element_size) > stack->cap) {
        realloc_stack(stack);
    }
    memcpy(stack->data + (stack->len - 1) * stack->element_size, data, stack->element_size);
    return stack->len;
}

size_t peek(arr_stack_t *stack, void *dest) {
    if (stack->len == 0) {
        return 0;
    }
    if ((stack->len - 1) <= 0) {
        return 0;
    }
    memcpy(dest, stack->data + (stack->len - 1) * stack->element_size, stack->element_size);
    return stack->len;
}

size_t pop(arr_stack_t *stack, void *dest) {
    if ((stack->len - 1) < 0) return 0;
    memcpy(dest, stack->data + (--stack->len) * stack->element_size, stack->element_size);
    return stack->len;
}