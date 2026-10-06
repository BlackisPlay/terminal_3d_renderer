#ifndef RAY_TRIANGLE_H
#define RAY_TRIANGLE_H

#include <stdbool.h>
#include "structs.h"

void normalize_vertices(TriangleArray* mesh,  Box* box);
float getLightLevel(float z, float y, TriangleArray* mesh);
void rotate_z(TriangleArray* mesh, TriangleArray* mesh_to_rotate, float angle);
void update_bounding_box(TriangleArray* mesh, Box* box);
bool isInside(float z, float y, Box* box);

#endif
