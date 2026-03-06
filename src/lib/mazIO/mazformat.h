// Constants for working with .MAZ-format files.
// Shared by import.h/export.h
// Created by Nokko on 2022-04-16.

#ifndef RAY_MAZE_MAZFORMAT_H
#define RAY_MAZE_MAZFORMAT_H

#include <stdint.h> // For int types
#include <stdlib.h> // For size_t


// More or less raw data, use a struct packed_chunk at higher levels.
struct maze_dump {
    int width;
    int height;
    size_t size;
    unsigned char *dump;
};

struct packed_chunk {
    uint32_t width;
    uint32_t height;
    size_t len;
    uint8_t packing_method;
    uint8_t *data;
};

// Constants! Need to be static to prevent linkage issues.

// .MAZ Format File Header
static const char MAZ_HEADER[8] = "\xe4\xe5maze<3";

// Packing Methods.
static const uint8_t PM_Packed = 0;
static const uint8_t PM_Unpacked = 1;
static const uint8_t PM_RESERVED = 63;

#endif //RAY_MAZE_MAZFORMAT_H
