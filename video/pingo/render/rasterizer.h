#pragma once

#include "texture.h"
#include "sprite.h"
#include "renderer.h"

/**
 * @brief Defines the type of filtering used when textures are resized and rotated
 * by a transformation. Nearest filtering just takes 1 texture sample in the
 * single pixel to which the source coordinate is transformed.
 * Bilinear filtering takes 4 samples on the source texture to compute a
 * weighted average of those 4 values based on the distance of the input
 * point.
 * Anisotropic filtering takes 4/16 samples from the source image
 * distributed uniformly over the area occupied by the destination pixel on
 * the source image and makes an average of those values.
 */

#define FILTERING_NEAREST
//#define FILTERING_BILINEAR
//#define FILTERING_ANISOTROPIC
//#define FILTERING_ANISOTROPICX2

/// @brief Draws a texture pixel-perfectly onto the renderer's framebuffer.
/// @param off Offset position on the framebuffer.
/// @param r Pointer to the renderer.
/// @param src Pointer to the source texture.
/// @return Status code indicating success or error.
int rasterizer_draw_pixel_perfect(Vec2i off, Renderer *r, Texture *src);

/// @brief Draws a texture pixel-perfectly and doubled in size onto the renderer's framebuffer.
/// @param off Offset position on the framebuffer.
/// @param r Pointer to the renderer.
/// @param src Pointer to the source texture.
/// @return Status code indicating success or error.
int rasterizer_draw_pixel_perfect_doubled(Vec2i off, Renderer *r, Texture *src);

/// @brief Draws a transformed texture onto the renderer's framebuffer.
/// @param t Transformation matrix.
/// @param r Pointer to the renderer.
/// @param src Pointer to the source texture.
/// @return Status code indicating success or error.
int rasterizer_draw_transformed(Mat4 t, Renderer *r, Texture *src);
