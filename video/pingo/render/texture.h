#pragma once

#include "../math/vec2.h"
#include "pixel.h"
#include "renderable.h"

/// @brief Texture structure representing a 2D texture with a size and frame buffer.
typedef struct {
    Vec2i size;       ///< Size of the texture
    Pixel *frameBuffer; ///< Pointer to the frame buffer
} Texture;

/// @brief Initializes a texture with a given size and buffer.
/// @param texture Pointer to the texture to initialize.
/// @param size Size of the texture.
/// @param buf Pointer to the buffer.
/// @return Status code indicating success or error.
extern int texture_init(Texture *texture, Vec2i size, Pixel *buf);

/// @brief Initializes a texture with a given size from an RGBA file.
/// @param texture Pointer to the texture to initialize.
/// @param size Size of the texture.
/// @param filename Name of the file to load the texture from.
/// @return Status code indicating success or error.
extern int texture_init_rgbafile(Texture *texture, Vec2i size, char *filename);

/// @brief Converts a texture to a renderable object.
/// @param texture Pointer to the texture.
/// @return Renderable object representing the texture.
extern Renderable texture_as_renderable(Texture *texture);

/// @brief Draws a pixel on the texture at a specified position.
/// @param texture Pointer to the texture.
/// @param pos Position to draw the pixel.
/// @param color Color of the pixel to draw.
extern void texture_draw(Texture *texture, Vec2i pos, Pixel color);

/// @brief Reads a pixel from the texture at a specified position.
/// @param texture Pointer to the texture.
/// @param pos Position to read the pixel from.
/// @return Pixel at the specified position.
extern Pixel texture_read(Texture *texture, Vec2i pos);

/// @brief Reads a pixel from the texture at a specified floating-point position.
/// @param texture Pointer to the texture.
/// @param pos Floating-point position to read the pixel from.
/// @return Pixel at the specified floating-point position.
extern Pixel texture_readF(Texture *texture, Vec2f pos);
