#include "sprite.h"
#include "../math/mat4.h"
#include "../render/rasterizer.h"
#include "renderer.h"
#include "state.h"

int render_sprite(void *_sprite, Mat4 transform, Renderer *renderer)
{
    IF_NULL_RETURN(_sprite, INIT_ERROR);
    IF_NULL_RETURN(renderer, INIT_ERROR);

    Sprite *sprite = (Sprite *)_sprite;

    /*
     * TODO: Implement for 3D?
     * if (mat4IsOnlyTranslation(&t)) {
     *     Vec2i off = {s->t.elements[2], s->t.elements[5]};
     *     rasterizer_draw_pixel_perfect(off,r, &s->frame);
     *     s->t = backUp;
     *     return 0;
     * }
    */
    rasterizer_draw_transformed(transform, renderer, &sprite->texture);
    return OK;
}

int sprite_init(Sprite *sprite, Texture texture)
{
    IF_NULL_RETURN(sprite, INIT_ERROR);

    sprite->texture = texture;
    sprite->renderable.render = &render_sprite;

    return OK;
}

int sprite_randomize(Sprite *sprite) {
    for (int x = 0; x < sprite->texture.size.x; x++)
        for (int y = 0; y < sprite->texture.size.y; y++)
            texture_draw(&sprite->texture, (Vec2i){x, y}, pixelRandom());

    return OK;
}
