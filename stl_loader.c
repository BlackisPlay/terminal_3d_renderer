#include "stl_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static uint32_t read_u32(const unsigned char *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float read_float(const unsigned char *p)
{
    uint32_t bits = read_u32(p);

    float value;
    memcpy(&value, &bits, sizeof(value));

    return value;
}

TriangleArray stl_load(const char *filename)
{
    TriangleArray result = {0};

    FILE *file = fopen(filename, "rb");

    if (!file) {
        perror(filename);
        return result;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return result;
    }

    long file_size = ftell(file);

    if (file_size < 84) {
        fprintf(stderr, "Invalid STL file\n");
        fclose(file);
        return result;
    }

    rewind(file);

    size_t size = (size_t)file_size;

    unsigned char *data = malloc(size);

    if (!data) {
        fprintf(stderr, "Out of memory\n");
        fclose(file);
        return result;
    }

    if (fread(data, 1, size, file) != size) {
        fprintf(stderr, "Failed to read STL file\n");
        free(data);
        fclose(file);
        return result;
    }

    fclose(file);

    /*
     * Binary STL:
     *
     * 80 bytes  header
     *  4 bytes  triangle count
     * 50 bytes  per triangle
     */
    uint32_t count = read_u32(data + 80);

    size_t expected_size = 84 + (size_t)count * 50;

    if (expected_size != size) {
        fprintf(stderr, "Not a valid binary STL file\n");
        free(data);
        return result;
    }

    Triangle *triangles =
        malloc((size_t)count * sizeof(Triangle));

    if (!triangles) {
        fprintf(stderr, "Out of memory for triangles\n");
        free(data);
        return result;
    }

    const unsigned char *p = data + 84;

    for (uint32_t i = 0; i < count; i++) {

        Triangle *triangle = &triangles[i];

        triangle->normal.x = read_float(p + 0);
        triangle->normal.y = read_float(p + 4);
        triangle->normal.z = read_float(p + 8);

        triangle->v1.x = read_float(p + 12);
        triangle->v1.y = read_float(p + 16);
        triangle->v1.z = read_float(p + 20);

        triangle->v2.x = read_float(p + 24);
        triangle->v2.y = read_float(p + 28);
        triangle->v2.z = read_float(p + 32);

        triangle->v3.x = read_float(p + 36);
        triangle->v3.y = read_float(p + 40);
        triangle->v3.z = read_float(p + 44);

        p += 50;
    }

    free(data);

    result.triangles = triangles;
    result.count = count;

    return result;
}

void stl_free(TriangleArray *mesh)
{
    free(mesh->triangles);

    mesh->triangles = NULL;
    mesh->count = 0;
}
