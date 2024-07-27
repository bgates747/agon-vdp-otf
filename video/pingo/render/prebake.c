#include <stdlib.h>

#include "prebake.h"
#include "mesh.h"
#include "../math/vec3.h"
#include "../math/mat4.h"

Renderable objectprebake_as_renderable(ObjectPrebake * objectprebake)
{
    return (Renderable){.renderableType = RENDERABLE_OBJECT_PREBAKE, .impl = objectprebake};
}