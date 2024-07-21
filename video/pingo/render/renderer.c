#include <string.h>
#include <stdio.h>
#include "../math/mat4.h"
#include "../render/state.h"
#include "renderer.h"
#include "pixel.h"
#include "depth.h"
#include "backend.h"

int renderer_init(Renderer *renderer, Vec2i size, Backend *backend)
{
    renderer->root_renderable = NULL;
    renderer->clear = true;
    renderer->clear_color = PIXELBLACK;
    renderer->backend = backend;
    renderer->backend->init(renderer, renderer->backend, (Vec4i){0, 0, 0, 0});

    int e = 0;
    e = texture_init(&renderer->framebuffer, size, backend->getFrameBuffer(renderer, backend));
    if (e) return e;

    return 0;
}

int renderer_render(Renderer *renderer)
{
    Backend *backend = renderer->backend;

    int pixels = renderer->framebuffer.size.x * renderer->framebuffer.size.y;
    memset(backend->getZetaBuffer(renderer, backend), 0, pixels * sizeof(PingoDepth));

    backend->beforeRender(renderer, backend);

    // Get current framebuffer from Backend
    renderer->framebuffer.frameBuffer = backend->getFrameBuffer(renderer, backend);

    // Clear draw buffer before rendering
    if (renderer->clear) {
        memset(backend->getFrameBuffer(renderer, backend), 0, pixels * sizeof(Pixel));
    }

    renderer->root_renderable->render(renderer->root_renderable, mat4Identity(), renderer);

    backend->afterRender(renderer, backend);

    return 0;
}

int renderer_set_root_renderable(Renderer *renderer, Renderable *root)
{
    IF_NULL_RETURN(renderer, SET_ERROR);
    IF_NULL_RETURN(root, SET_ERROR);

    renderer->root_renderable = root;
    return 0;
}
