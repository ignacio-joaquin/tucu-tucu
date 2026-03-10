#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lib/mazIO/maze.h"
#include "lib/mazIO/import.h"
#include "lib/mazIO/mazformat.h"

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
    const int width = 16;
    const int height = 8;
    int *data = calloc(width * height, sizeof(int));
    if (!data) {
        fprintf(stderr, "calloc failed\n");
        return 2;
    }

    maze_t m = {data, width, height};

    // build the maze
    carve_maze(&m);

    // Prepare header (33 bytes expected by read_maz)
    unsigned char header[33];
    memset(header, 0, sizeof(header));
    memcpy(header, MAZ_HEADER, 8);
    // store width and height as little-endian uint32 at offsets 24 and 28
    *(uint32_t *)(header + 24) = (uint32_t) width;
    *(uint32_t *)(header + 28) = (uint32_t) height;
    header[32] = (uint8_t) PM_Unpacked; // unpacked: one byte per cell

    const char *outpath = "tests/generated.maz";
    FILE *out = fopen(outpath, "wb");
    if (!out) { perror("fopen"); free(data); return 1; }

    if (fwrite(header, 1, sizeof(header), out) != sizeof(header)) {
        perror("fwrite header"); fclose(out); free(data); return 1;
    }

    // write cell bytes (low nibble contains cell value)
    for (int i = 0; i < width * height; ++i) {
        unsigned char v = (unsigned char)(m.data[i] & 0x0F);
        if (fwrite(&v, 1, 1, out) != 1) { perror("fwrite data"); fclose(out); free(data); return 1; }
    }
    fclose(out);

    free(data);

    // Now read it back using existing read_maz
    struct maze *mz = read_maz(outpath);
    if (!mz) {
        fprintf(stderr, "read_maz failed for %s\n", outpath);
        return 1;
    }

    print_maze_compact(mz);

    free(mz->data);
    free(mz);
    return 0;
}
