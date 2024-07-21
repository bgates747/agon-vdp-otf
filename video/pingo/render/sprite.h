#pragma once

#include "renderable.h"
#include "texture.h"

/// @brief Sprite structure representing a renderable sprite with a texture.
typedef struct {
    Renderable renderable; ///< Base renderable structure for the sprite
    Texture texture;       ///< Texture associated with the sprite
} Sprite;

/// @brief Initializes a sprite with the given texture.
/// @param sprite Pointer to the sprite to initialize.
/// @param texture Texture to associate with the sprite.
/// @return Status code indicating success or error.
extern int sprite_init(Sprite *sprite, Texture texture);

/// @brief Randomizes the pixels of the sprite's texture.
/// @param sprite Pointer to the sprite.
/// @return Status code indicating success or error.
extern int sprite_randomize(Sprite *sprite);
