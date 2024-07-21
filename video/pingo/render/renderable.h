#pragma once

#include "../math/mat4.h"

typedef struct Renderer Renderer;

/// @brief A basic type which provides a render function pointer.
typedef struct {
    /// @brief Pointer to the render function.
    /// @param renderable Pointer to the renderable object.
    /// @param transform Transformation matrix for rendering.
    /// @param renderer Pointer to the renderer.
    /// @return Status code indicating success or error.
    int (*render)(void *renderable, Mat4 transform, Renderer *renderer);
} Renderable;
