#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lib/mazIO/maze.h"
#include "lib/mazIO/stack.h"
#include "lib/mazIO/import.h"

static void print_maze(struct maze *m) {
    int w = m->width;
    int h = m->height;
    // top border
    for (int x = 0; x < w; ++x) {
        printf("+");
        int cell = m->data[0 * w + x];
        if (cell & N) printf("   "); else printf("---");
    }
    printf("+\n");

    for (int y = 0; y < h; ++y) {
        // cell row
        for (int x = 0; x < w; ++x) {
            int cell = m->data[y * w + x];
            if (cell & W) printf(" "); else printf("|");
            printf("   ");
        }
        int last = m->data[y * w + (w - 1)];
        if (last & E) printf(" "); else printf("|");
        printf("\n");

        // separator row
        for (int x = 0; x < w; ++x) {
            printf("+");
            int cell = m->data[y * w + x];
            if (cell & S) printf("   "); else printf("---");
        }
        printf("+\n");
    }
}

// Compact printer: prints two chars per cell using '_' for floor and '|' for wall.
static void print_maze_compact(struct maze *m) {
    int w = m->width;
    int h = m->height;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int cell = m->data[y * w + x];
            char bottom = (cell & S) ? ' ' : '_';
            char right = (cell & E) ? ' ' : '|';
            putchar(bottom);
            putchar(right);
        }
        putchar('\n');
    }
}

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
    
    // Parse provided coolmaze.maz and ensure it loads
    struct maze *mz = read_maz("tests/coolmaze.maz");
    if (!mz) {
        fprintf(stderr, "read_maz failed for tests/coolmaze.maz\n");
        return 1;
    }
    if (mz->width <= 0 || mz->height <= 0 || mz->data == NULL) {
        fprintf(stderr, "read_maz: invalid maze data\n");
        return 1;
    }
    // Simple sanity: at least one cell must be non-NONE
    int any = 0;
    for (int i = 0; i < mz->width * mz->height; ++i) if (mz->data[i] != NONE) { any = 1; break; }
    if (!any) {
        fprintf(stderr, "read_maz: all cells NONE\n");
        free(mz->data);
        free(mz);
        return 1;
    }

    print_maze_compact(mz);
    free(mz->data);
    free(mz);
    return 0;
}
