#pragma once

#include "../math/vec4.h"

/**
 * @brief Provides a common interface to multiple graphical backends.
 */

typedef struct Renderer Renderer;
typedef struct Pixel Pixel;
typedef struct PingoDepth PingoDepth;
typedef struct Texture Texture;

/// @brief Backend structure defining function pointers for graphical backend operations.
typedef struct Backend {
    /// @brief Called on initialization and re-initialization.
    /// @param renderer Pointer to the Renderer.
    /// @param backend Pointer to the Backend.
    /// @param rect Rectangle defining the viewport or render area.
    void (*init)(Renderer *renderer, struct Backend *backend, Vec4i rect);

    /// @brief Called before starting rendering.
    /// @param renderer Pointer to the Renderer.
    /// @param backend Pointer to the Backend.
    void (*beforeRender)(Renderer *renderer, struct Backend *backend);

    /// @brief Called after having finished a render.
    /// @param renderer Pointer to the Renderer.
    /// @param backend Pointer to the Backend.
    void (*afterRender)(Renderer *renderer, struct Backend *backend);

    /// @brief Should return the address of the frame buffer.
    /// @param renderer Pointer to the Renderer.
    /// @param backend Pointer to the Backend.
    /// @return Pointer to the frame buffer (height * width * sizeof(Pixel)).
    Pixel *(*getFrameBuffer)(Renderer *renderer, struct Backend *backend);

    /// @brief Should return the address of the zeta (depth) buffer.
    /// @param renderer Pointer to the Renderer.
    /// @param backend Pointer to the Backend.
    /// @return Pointer to the zeta buffer (height * width * sizeof(Pixel)).
    PingoDepth *(*getZetaBuffer)(Renderer *renderer, struct Backend *backend);
} Backend;
