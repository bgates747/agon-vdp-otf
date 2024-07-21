#pragma once

#include "texture.h"

/// @brief Material structure associating a texture with a material.
typedef struct {
    Texture *texture; ///< Pointer to the texture associated with the material
} Material;

/// @brief Initializes a material with a given texture.
/// @param material Pointer to the material to initialize.
/// @param texture Pointer to the texture to associate with the material.
/// @return Status code indicating success or error.
int material_init(Material *material, Texture *texture);
