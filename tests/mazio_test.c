#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lib/mazIO/maze.h"
#include "lib/mazIO/stack.h"

int main(void) {
    int width = 10, height = 10;
    int *data = calloc(width * height, sizeof(int));
    if (!data) {
        fprintf(stderr, "calloc failed\n");
        return 2;
    }

    maze_t m = {data, width, height};

    // Test fill_maze sets all cells to NONE
    fill_maze(&m);
    for (int i = 0; i < width * height; ++i) {
        if (m.data[i] != NONE) {
            fprintf(stderr, "fill_maze: expected NONE at index %d, got %d\n", i, m.data[i]);
            free(data);
            return 1;
        }
    }

    // Test carve_maze changes some cells
    carve_maze(&m);
    int changed = 0;
    for (int i = 0; i < width * height; ++i) {
        if (m.data[i] != NONE) ++changed;
    }
    if (changed == 0) {
        fprintf(stderr, "carve_maze: no cells carved\n");
        free(data);
        return 1;
    }

    // Test get_cell points to expected element
    pos_t p = {3, 4};
    int *cell_ptr = get_cell(&m, &p);
    if (cell_ptr != &m.data[p.oy * m.width + p.ox]) {
        fprintf(stderr, "get_cell: pointer mismatch\n");
        free(data);
        return 1;
    }

    // Basic stack smoke test
    arr_stack_t *s = new_stack(sizeof(pos_t), 4);
    if (!s) { fprintf(stderr, "new_stack failed\n"); free(data); return 2; }
    pos_t a = {1,2}, b = {3,4}, out;
    push(s, &a);
    push(s, &b);
    peek(s, &out);
    if (out.ox != b.ox || out.oy != b.oy) { fprintf(stderr, "stack peek failed\n"); free_stack(s); free(data); return 1; }
    pop(s, &out);
    if (out.ox != b.ox || out.oy != b.oy) { fprintf(stderr, "stack pop failed\n"); free_stack(s); free(data); return 1; }
    pop(s, &out);
    if (out.ox != a.ox || out.oy != a.oy) { fprintf(stderr, "stack pop failed 2\n"); free_stack(s); free(data); return 1; }
    free_stack(s);

    free(data);
    puts("OK");
    return 0;
}
