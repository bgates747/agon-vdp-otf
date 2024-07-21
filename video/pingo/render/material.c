#include "material.h"
#include "../render/state.h"

int material_init(Material *material, Texture *texture)
{
    IF_NULL_RETURN(material, INIT_ERROR);
    IF_NULL_RETURN(texture, INIT_ERROR);

    material->texture = texture;
    return OK;
}
