#pragma once

#include "../math/mat4.h"

#include "mesh.h"
#include "renderable.h"
#include "material.h"
#include "prebake.h"

typedef struct Object {
    Mesh *mesh;
    Mat4 transform;
    Material *material;
    Vec2f *textCoord;
    Prebake *prebake;
} Object;

Renderable object_as_renderable(Object * object);