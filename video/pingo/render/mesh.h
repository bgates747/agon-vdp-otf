#pragma once

#include <stdint.h>
#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/mat4.h"

#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

typedef struct Mesh {
    int indexes_count;
    uint16_t *pos_indices;
    uint16_t *tex_indices;
    uint16_t *nor_indices;
    Vec3f *positions;
    Vec2f *textCoord;
    Vec3f *normals;
    float *diffuseLight;  // Precomputed diffuse light values

    // Precomputed texture coordinates for each vertex
    Vec2f *precomputedTCA;
    Vec2f *precomputedTCB;
    Vec2f *precomputedTCC;
} Mesh;


// Function declarations
void computeMeshNormals(Mesh *mesh);
void precomputeLighting(Mesh *mesh, Vec3f lightDirection);
void precomputeTextureCoordinates(Mesh *mesh);