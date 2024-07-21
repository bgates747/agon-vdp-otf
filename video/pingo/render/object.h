#pragma once

#include "renderable.h"

typedef struct Mesh Mesh;
typedef struct Material Material;

/// @brief Object structure representing a 3D object with a mesh and material.
typedef struct Object {
    Renderable renderable;  ///< Base renderable structure for the object
    Mesh *mesh;             ///< Pointer to the mesh associated with the object
    Material *material;     ///< Pointer to the material associated with the object
} Object;

/// @brief Initializes an object with a given mesh and material.
/// @param object Pointer to the object to initialize.
/// @param mesh Pointer to the mesh to associate with the object.
/// @param material Pointer to the material to associate with the object.
/// @return Status code indicating success or error.
extern int object_init(Object *object, Mesh *mesh, Material *material);
