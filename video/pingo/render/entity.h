#pragma once

#include "array.h"
#include "renderable.h"
#include <stdbool.h>

/// @brief Entity is a renderable associated with a transform which is renderable at a
/// specific position by the renderer.
typedef struct {
    Renderable renderable;            ///< The base renderable structure for the entity
    Renderable *entity_renderable;    ///< Pointer to the actual renderable object
    Mat4 transform;                   ///< Transformation matrix for the entity
    bool visible;                     ///< Visibility flag for the entity
    Array children_entities;          ///< Array of child entities
} Entity;

/// @brief Initializes an entity with a renderable and transform.
/// @param entity Pointer to the entity to initialize.
/// @param renderable Pointer to the renderable object.
/// @param transform Transformation matrix for the entity.
/// @return Status code indicating success or error.
extern int entity_init(Entity *entity, Renderable *renderable, Mat4 transform);

/// @brief Initializes an entity with a renderable, transform, and child entities.
/// @param entity Pointer to the entity to initialize.
/// @param renderable Pointer to the renderable object.
/// @param transform Transformation matrix for the entity.
/// @param children Array of child entities.
/// @param children_count Number of child entities.
/// @return Status code indicating success or error.
extern int entity_init_children(Entity *entity, Renderable *renderable, Mat4 transform, Entity children[], size_t children_count);
