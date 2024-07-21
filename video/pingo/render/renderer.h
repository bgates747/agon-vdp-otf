#pragma once

#include "pixel.h"
#include "texture.h"
#include "renderable.h"
#include <stdbool.h>
#include "../math/mat4.h"

typedef struct Backend Backend;

/// @brief Renderer structure representing the rendering context.
typedef struct Renderer {
    Renderable *root_renderable; ///< Root renderable object

    Texture framebuffer;         ///< Framebuffer for rendering
    Pixel clear_color;           ///< Clear color for the framebuffer
    bool clear;                  ///< Flag indicating whether to clear the framebuffer

    Mat4 camera_projection;      ///< Camera projection matrix
    Mat4 camera_view;            ///< Camera view matrix

    Backend *backend;            ///< Pointer to the backend implementation
} Renderer;

/// @brief Initializes the renderer with a given size and backend.
/// @param renderer Pointer to the renderer to initialize.
/// @param size Size of the framebuffer.
/// @param backend Pointer to the backend implementation.
/// @return Status code indicating success or error.
extern int renderer_init(Renderer *renderer, Vec2i size, Backend *backend);

/// @brief Renders the scene using the renderer.
/// @param renderer Pointer to the renderer.
/// @return Status code indicating success or error.
extern int renderer_render(Renderer *renderer);

/// @brief Sets the root renderable object for the renderer.
/// @param renderer Pointer to the renderer.
/// @param root Pointer to the root renderable object.
/// @return Status code indicating success or error.
extern int renderer_set_root_renderable(Renderer *renderer, Renderable *root);
