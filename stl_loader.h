#ifndef STL_LOADER_H
#define STL_LOADER_H

#include "structs.h"

TriangleArray stl_load(const char *filename);

void stl_free(TriangleArray *mesh);

#endif
