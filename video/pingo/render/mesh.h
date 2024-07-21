#pragma once

#include <stdint.h>
#include "../math/vec2.h"
#include "../math/vec3.h"

/// @brief Mesh structure representing a 3D mesh with positions and texture coordinates.
typedef struct {
    int indexes_count;       ///< Number of indices in the mesh
    uint16_t *pos_indices;   ///< Pointer to position indices
    uint16_t *tex_indices;   ///< Pointer to texture indices
    Vec3f *positions;        ///< Pointer to position vectors (3D coordinates)
    Vec2f *textCoord;        ///< Pointer to texture coordinates (2D coordinates)
} Mesh;
