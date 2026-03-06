//
// Maze generation code - display-agnostic.
// Created by Nokko on 2022-04-06.
//

#ifndef RAY_MAZE_MAZE_H
#define RAY_MAZE_MAZE_H

#include <stdbool.h>
#include "stack.h"

// A maze is represented as pointer to the beginning of a `data` array,
// and that array's width and height.
typedef struct maze {
    int *data;
    int width;
    int height;
} maze_t;

// Simple Vec2 thing
typedef struct {int ox; int oy;} pos_t;

// Openings in each maze cell are represented as a bitfield:
// 0b1000 = 8, north
#define N 8
// 0b0100 = 4, east
#define E 4
// 0b0010 = 2, south
#define S 2
// 0b0001 = 1, west
#define W 1

// 0b1111 = 15, every way is open
#define ALL 15
// 0b0000 = 0,  every way is walled-off
#define NONE 0

typedef struct neighbour {
    char ox;
    char oy;
    int direction;
    int *data;
} neighbour_t;

neighbour_t get_neighbour(int ox, int oy, maze_t *m);

void fill_maze(maze_t *m);

void carve(int ox, int oy, maze_t *m);

void carve_maze(maze_t *m);

bool carve_from(struct maze *m, pos_t *place, struct neighbour *n);

bool carve_step(struct maze *m, arr_stack_t *stack, pos_t *current);

bool carve_step_fishbone(struct maze *m, arr_stack_t *stack, pos_t *current);

int *get_cell(struct maze *m, pos_t *pos);

#endif //RAY_MAZE_MAZE_H
