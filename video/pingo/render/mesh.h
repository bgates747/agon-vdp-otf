#pragma once

#include <stdint.h>
#include <math.h>
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
    float *diffuseLight;  // Store precomputed diffuse light values
} Mesh;

// Function to compute normals for the mesh
void computeMeshNormals(Mesh *mesh);

// New function to precompute lighting
void precomputeLighting(Mesh *mesh, Vec3f lightDirection);
