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

typedef struct ObjectPrebake {
    Mesh *mesh;
    Mat4 transform;
    Material *material;
    Vec2f *textCoord;
    Prebake *prebake;
} ObjectPrebake;

Renderable objectprebake_as_renderable(ObjectPrebake * objectprebake);
