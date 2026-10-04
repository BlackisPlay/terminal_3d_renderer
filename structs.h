#ifndef STRUCTS_H
#define STRUCTS_H

#include <stddef.h>

typedef struct {
    float z_min;
    float y_min;
    float z_max;
    float y_max;
} Box;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    Vec3 normal;
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
} Triangle;

typedef struct {
    Triangle *triangles;
    size_t count;
} TriangleArray;

#endif

