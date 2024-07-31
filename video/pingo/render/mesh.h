#pragma once

#include <stdint.h>

#include "../math/vec2.h"
#include "../math/vec3.h"

typedef struct Vertices {
    uint16_t *pos_indices;
    Vec3f *positions;
} Vertices;

typedef struct UVs {
    uint16_t *tex_indices;
    Vec2f *textCoord;
} UVs;

typedef struct Normals {
    uint16_t *nor_indices;
    Vec3f *normals;
} Normals;

typedef struct Mesh {
    int indexes_count;
    Vertices *vertices;
    UVs *uvs;
    Normals *normals;
} Mesh;


