#include <stddef.h> 
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "zbuffer.h"

// Constructor for ZBuffer
ZBuffer* initZBuffer(const Camera* camera) {
    ZBuffer* zbuffer = (ZBuffer*)malloc(sizeof(ZBuffer));
    if (zbuffer == NULL) {
        return NULL; // Memory allocation failed
    }

    zbuffer->screen_width = camera->screen_width;

    zbuffer->depths = (float*)malloc(zbuffer->screen_width * sizeof(float));
    zbuffer->texture_id = (uint16_t*)malloc(zbuffer->screen_width * sizeof(uint16_t));
    zbuffer->u = (float*)malloc(zbuffer->screen_width * sizeof(float));
    zbuffer->strip_height = (float*)malloc(zbuffer->screen_width * sizeof(float));

    if (zbuffer->depths == NULL || zbuffer->texture_id == NULL || zbuffer->u == NULL || zbuffer->strip_height == NULL) {
        free(zbuffer->depths);
        free(zbuffer->texture_id);
        free(zbuffer->u);
        free(zbuffer->strip_height);
        free(zbuffer);
        return NULL; // Memory allocation failed
    }

    // Initialize arrays to default values (if needed)
    memset(zbuffer->depths, 0, zbuffer->screen_width * sizeof(float));
    memset(zbuffer->texture_id, 0, zbuffer->screen_width * sizeof(uint16_t));
    memset(zbuffer->u, 0, zbuffer->screen_width * sizeof(float));
    memset(zbuffer->strip_height, 0, zbuffer->screen_width * sizeof(float));

    return zbuffer;
}

// Destructor for ZBuffer
void destroyZBuffer(ZBuffer* zbuffer) {
    if (zbuffer != NULL) {
        free(zbuffer->depths);
        free(zbuffer->texture_id);
        free(zbuffer->u);
        free(zbuffer->strip_height);
        free(zbuffer);
    }
}