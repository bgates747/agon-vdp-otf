#include "entity.h"
#include "../math/mat4.h"
#include "../render/array.h"
#include "state.h"
#include <stddef.h>

int entity_render(void *entity_ptr, Mat4 transform, Renderer *renderer)
{
    Entity *entity = (Entity *)entity_ptr;
    IF_NULL_RETURN(entity, RENDER_ERROR);
    IF_NULL_RETURN(renderer, RENDER_ERROR);

    if (!entity->visible)
        return OK;

    Mat4 new_transform = mat4MultiplyM(&entity->transform, &transform);

    for (int i = 0; i < entity->children_entities.size; i++) {
        Entity *child_entity = &entity->children_entities.data[i];
        child_entity->renderable.render(child_entity, new_transform, renderer);
    }

    Renderable *renderable = entity->entity_renderable;

    return renderable->render(renderable, new_transform, renderer);
}

int entity_init(Entity *entity, Renderable *renderable, Mat4 transform)
{
    // Check for null pointers to avoid segmentation faults
    IF_NULL_RETURN(entity, INIT_ERROR);
    IF_NULL_RETURN(renderable, INIT_ERROR);

    // Initialize entity fields
    entity->entity_renderable = renderable;
    entity->renderable.render = &entity_render;
    entity->transform = transform;
    entity->visible = true;

    // Initialize children entities array with zero initial size
    array_init(&entity->children_entities, 0, 0);

    return OK;
}

int entity_init_children(Entity *entity, Renderable *renderable, Mat4 transform, Entity children[], size_t children_count)
{
    // Check for null pointers to avoid segmentation faults
    IF_NULL_RETURN(entity, INIT_ERROR);
    IF_NULL_RETURN(renderable, INIT_ERROR);

    // Initialize entity fields
    entity->entity_renderable = renderable;
    entity->renderable.render = &entity_render;
    entity->transform = transform;
    entity->visible = true;

    // Initialize children entities array with provided children
    array_init(&entity->children_entities, children_count, children);

    return OK;
}
