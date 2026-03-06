//
// Created by Nokko on 2022-04-16.
//

#ifndef RAY_MAZE_IMPORT_H
#define RAY_MAZE_IMPORT_H

#include "mazformat.h"
#include "maze.h"

// Read in a `.maz`-format file.
// Errors are not handled - returns a NULL ptr on error.
struct maze *read_maz(const char *filename);

// Read in a `.rawmaz` maze dump. No error handling here, either.
struct maze *read_dump(const char *filename);

#endif //RAY_MAZE_IMPORT_H