#include "texture.h"
#include "../render/state.h"
#include <stdio.h>

int texture_init(Texture *texture, Vec2i size, Pixel *buf)
{
    if (size.x * size.y == 0)
        return 1; // 0 sized rect

    if (buf == NULL)
        return 2; // null ptr buffer

    texture->frameBuffer = buf;
    texture->size = size;

    return OK;
}

void texture_draw(Texture *texture, Vec2i pos, Pixel color)
{
    texture->frameBuffer[pos.x + pos.y * texture->size.x] = color;
}

Pixel texture_read(Texture *texture, Vec2i pos)
{
    return texture->frameBuffer[pos.x + pos.y * texture->size.x];
}

Pixel texture_readF(Texture *texture, Vec2f pos)
{
    uint16_t x = (uint16_t)(pos.x * texture->size.x) % texture->size.x;
    uint16_t y = (uint16_t)(pos.y * texture->size.y) % texture->size.x;
    uint32_t index = x + y * texture->size.x;
    return texture->frameBuffer[index];
}
