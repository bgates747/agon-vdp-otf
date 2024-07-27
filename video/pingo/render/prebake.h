#pragma once

#include "../math/mat4.h"
#include "../math/vec2.h"
#include "../math/vec3.h"

#include "mesh.h"
#include "renderable.h"
#include "material.h"

typedef struct Prebake {
    Vec3f position;   // Transformed vertex positions in scene space
    Vec3f normal;     // Transformed normal vector for lighting calculations
    Vec2f textCoord;  // Texture coordinates for the vertex
    float light;      // Precomputed lighting value
} Prebake;

void copyPositions(Prebake *prebake, Mesh *mesh);
void copyNormals(Prebake *prebake, Mesh *mesh);
void copyTexCoords(Prebake *prebake, Mesh *mesh);

void prebakePositions(Prebake *prebake, Mesh *mesh, Mat4 *transform);
void prebakeNormals(Prebake *prebake, Mesh *mesh);
void prebakeLight(Prebake *prebake, Mesh *mesh);
void prebakeTexCoords(Prebake *prebake, Mesh *mesh, Vec2f *textCoord);
