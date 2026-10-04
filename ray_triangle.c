#include "ray_triangle.h"
#include <float.h>
#include <math.h>

#define MAX_VERTEX_DIST 14.f

void update_bounding_box(TriangleArray *mesh, Box* box)
{
    if (mesh->count == 0)
        return;

    Vec3 *v = &mesh->triangles[0].v1;

    float min_z = v->z;
    float max_z = v->z;
    float min_y = v->y;
    float max_y = v->y;

    for (size_t i = 0; i < mesh->count; i++) {
        Triangle *t = &mesh->triangles[i];

        Vec3 *vertices[] = {
            &t->v1,
            &t->v2,
            &t->v3
        };

        for (int j = 0; j < 3; j++) {
            v = vertices[j];

            if (v->z < min_z)
                min_z = v->z;

            if (v->z > max_z)
                max_z = v->z;

            if (v->y < min_y)
                min_y = v->y;

            if (v->y > max_y)
                max_y = v->y;
        }
    }

    box->z_min = min_z;
    box->z_max = max_z;
    box->y_min = min_y;
    box->y_max = max_y;
}

void rotate_z(TriangleArray* mesh, TriangleArray* mesh_to_rotate, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);

    for (size_t i = 0; i < mesh->count; i++) {
        Triangle *t = &mesh->triangles[i];
        Triangle* t_rot = &mesh_to_rotate->triangles[i];

        Vec3 *vertices[] = {
            &t->v1,
            &t->v2,
            &t->v3,

            &t_rot->v1,
            &t_rot->v2,
            &t_rot->v3
        };

        for (int j = 0; j < 3; j++) {
            float x = vertices[j]->x;
            float y = vertices[j]->y;

            vertices[j + 3]->x = x * c - y * s;
            vertices[j + 3]->y = x * s + y * c;
        }

        float x = t->normal.x;
        float y = t->normal.y;

        t_rot->normal.x = x * c - y * s;
        t_rot->normal.y = x * s + y * c;
    }
}

static float cross2d(float az, float ay, float bz, float by)
{
    return az * by - ay * bz;
}

static int point_in_triangle(float z, float y, const Triangle *t)
{
    float c1 = cross2d(
        t->v2.z - t->v1.z,
        t->v2.y - t->v1.y,
        z - t->v1.z,
        y - t->v1.y
    );

    float c2 = cross2d(
        t->v3.z - t->v2.z,
        t->v3.y - t->v2.y,
        z - t->v2.z,
        y - t->v2.y
    );

    float c3 = cross2d(
        t->v1.z - t->v3.z,
        t->v1.y - t->v3.y,
        z - t->v3.z,
        y - t->v3.y
    );

    return (c1 >= 0.0f && c2 >= 0.0f && c3 >= 0.0f) ||
           (c1 <= 0.0f && c2 <= 0.0f && c3 <= 0.0f);
}

static float triangle_x_at_zy(float z, float y, const Triangle *t)
{

    float nx = t->normal.x;
    float ny = t->normal.y;
    float nz = t->normal.z;

    if (fabsf(nx) < 1e-8f)
        return NAN;

    return t->v1.x -
           (ny * (y - t->v1.y) +
            nz * (z - t->v1.z)) / nx;
}


float getLightLevel(float z, float y, TriangleArray *mesh)
{
    Triangle *closest = NULL;
    float closest_x = -FLT_MAX;

    for (size_t i = 0; i < mesh->count; i++) {
        Triangle *t = &mesh->triangles[i];

        if (!point_in_triangle(z, y, t))
            continue;

        float x = triangle_x_at_zy(z, y, t);

        if (!isfinite(x))
            continue;

        if (x > closest_x) {
            closest_x = x;
            closest = t;
        }
    }

    if (closest == NULL)
        return -1.0f;

    float nx = closest->normal.x;
    float ny = closest->normal.y;
    float nz = closest->normal.z;

    float length = sqrtf(nx * nx +
                         ny * ny +
                         nz * nz);

    if (length == 0.0f)
        return 0.0f;

    nx /= length;
    ny /= length;
    nz /= length;

    float light = nx * 0.577f +
                  ny * 0.577f -
                  nz * 0.577f;

    if(light < 0.f) light = 0.f;

    light *= 24.f;

    light = round(light);
    if(light < 0) light = 0;

    return light;
}

bool isInside(float z, float y, Box* box){
    return z >= box->z_min && z <= box->z_max && y >= box->y_min && y <= box->y_max;
}

static float dist_squared(const Vec3 *p)
{
    return p->x * p->x +
           p->y * p->y +
           p->z * p->z;
}

static void scale_vec3(Vec3 *v, float scale)
{
    v->x *= scale;
    v->y *= scale;
    v->z *= scale;
}

static void center_mesh(TriangleArray *mesh, Box* box)
{
    if (mesh->count == 0)
        return;

    Vec3 min = mesh->triangles[0].v1;
    Vec3 max = mesh->triangles[0].v1;

    for (size_t i = 0; i < mesh->count; i++) {
        Vec3 *vertices[] = {
            &mesh->triangles[i].v1,
            &mesh->triangles[i].v2,
            &mesh->triangles[i].v3
        };

        for (int j = 0; j < 3; j++) {
            Vec3 *v = vertices[j];

            if (v->x < min.x) min.x = v->x;
            if (v->y < min.y) min.y = v->y;
            if (v->z < min.z) min.z = v->z;

            if (v->x > max.x) max.x = v->x;
            if (v->y > max.y) max.y = v->y;
            if (v->z > max.z) max.z = v->z;
        }
    }

    Vec3 center = {
        (min.x + max.x) / 2.0f,
        (min.y + max.y) / 2.0f,
        (min.z + max.z) / 2.0f
    };

    for (size_t i = 0; i < mesh->count; i++) {
        Vec3 *vertices[] = {
            &mesh->triangles[i].v1,
            &mesh->triangles[i].v2,
            &mesh->triangles[i].v3
        };

        for (int j = 0; j < 3; j++) {
            vertices[j]->x -= center.x;
            vertices[j]->y -= center.y;
            vertices[j]->z -= center.z;
        }
    }

    box->z_min = min.z - center.z;
    box->y_min = min.y - center.y;
    box->z_max = max.z - center.z;
    box->y_max = max.y - center.y;
}

void normalize_vertices(TriangleArray *mesh, Box* box)
{
    if (mesh->count == 0)
        return;

    center_mesh(mesh, box);

    float furthest_point = 0.0f;

    for (size_t i = 0; i < mesh->count; i++) {

        mesh->triangles[i].v1.z *= -1.f;
        mesh->triangles[i].v2.z *= -1.f;
        mesh->triangles[i].v3.z *= -1.f;
        mesh->triangles[i].normal.z *= -1.f;

        float d1 = dist_squared(&mesh->triangles[i].v1);
        float d2 = dist_squared(&mesh->triangles[i].v2);
        float d3 = dist_squared(&mesh->triangles[i].v3);

        if (d1 > furthest_point)
            furthest_point = d1;

        if (d2 > furthest_point)
            furthest_point = d2;

        if (d3 > furthest_point)
            furthest_point = d3;
    }

    if (furthest_point == 0.0f)
        return;

    float scale = MAX_VERTEX_DIST / sqrtf(furthest_point);

    for (size_t i = 0; i < mesh->count; i++) {
        scale_vec3(&mesh->triangles[i].v1, scale);
        scale_vec3(&mesh->triangles[i].v2, scale);
        scale_vec3(&mesh->triangles[i].v3, scale);
    }

    box->z_min *= scale;
    box->y_min *= scale;
    box->z_max *= scale;
    box->y_max *= scale;
}
