#pragma once

#include <stdint.h>

#include "../math/vec2.h"
#include "../math/vec3.h"

typedef struct Vertex {
    uint16_t *pos_indices;
    Vec3f *positions;
} Vertex;

typedef struct UV {
    uint16_t *tex_indices;
    Vec2f *textCoord;
} UV;

typedef struct Normal {
    uint16_t *nor_indices;
    Vec3f *normals;
} Normal;

typedef struct Mesh {
    int indexes_count;
    Vertex *vertices;
    UV *uvs;
    Normal *normals;
} Mesh;