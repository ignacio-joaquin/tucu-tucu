//
// Created by Nokko on 2022-04-16.
//

#include <stdio.h> // For file operations
#include <string.h> // strcmp()
#include <stdint.h>
#include <stdbool.h>
#include "import.h"

// Read in a `.rawmaz` maze dump. No error handling here.
struct maze *read_dump(const char *filename) {

    return NULL;
}

// Unpack maze data from a .MAZ file and produce a maze.
struct maze *unpack_maz(struct packed_chunk *chunk) {
    struct maze *m = malloc(sizeof(struct maze));
    m->data = malloc(sizeof(int) * chunk->width * chunk->height);
    m->width = (int) chunk->width;
    m->height = (int) chunk->height;
    size_t mdata_len = chunk->width * chunk->height;

    bool packed = (chunk->packing_method == PM_Packed);
    int nibble_mask = 15;   // 00001111, low nibble
    if (packed) {
        nibble_mask ^= 255; // 11110000, high nibble
    }

    // Every loop, write 1 or 2 maze cells, consume 1 byte of data.
    uint8_t *d = chunk->data;
    for (int *mp = m->data; mp < m->data + mdata_len; mp++, d++) {
        *mp = (nibble_mask & *d); // First nibble
        if (packed) {
            *mp >>= 4;
            mp++;
        }

        if (packed && mp < m->data + mdata_len) {
            *mp = ((nibble_mask ^ 255) & *d); // Second nibble (if packed), no shift
        }
    }

    free(chunk->data);
    return m;
}

// Alloc enough memory to read in the data from a .MAZ file,
// read it, write the length out, close the file.
void read_data(FILE *f, struct packed_chunk *chunk) {
    // Figure out the length:
    switch (chunk->packing_method) {
        case PM_Packed:
            // Same ol' ceil division trick.
            chunk->len = (chunk->width * chunk->height + 1) / 2;
            break;
        case PM_Unpacked:
            chunk->len = (chunk->width * chunk->height);
            break;
        default:
            break;
    }
    uint8_t *data = malloc(chunk->len * sizeof(uint8_t));
    fread(data, sizeof(uint8_t), chunk->len, f);
    chunk->data = data;
    fclose(f);
}

// Read in a `.maz`-format file.
// Errors are not handled - returns a NULL ptr on error.
struct maze *read_maz(const char *filename) {
    // Alloc buffer for header
    uint8_t *header = malloc(33);

    // Open file
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        free(header);
        return NULL;
    }

    // Read header
    fread(header, sizeof(uint8_t), 33, f);

    // Make sure this is a .MAZ file
    bool header_matches = (strncmp((char *) header, MAZ_HEADER, 8) == 0);
    if (!header_matches) {
        free(header);
        return NULL;
    }

    // Read Dimensions Section
    struct packed_chunk chunk = {
            .width = *(uint32_t *) (header + 24),
            .height = *(uint32_t *) (header + 28),
            .packing_method = *(uint8_t *) (header + 32)
    };
    free(header);

    read_data(f, &chunk);

    return unpack_maz(&chunk);
}

