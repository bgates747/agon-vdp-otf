#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"
#include "camera.h"

#ifndef ZBUFFER_H
#define ZBUFFER_H

typedef struct ZBuffer {
    fixed8_8 *depths;          // Array of depth values for each ray
    uint16_t *texture_id;      // Texture ID for the texture
    float *u;                  // U component of texture UV coordinates
    fixed8_8 *strip_height;    // Height of the texture strip at the distance of the intersection
    int screen_width;          // Screen width in pixels (to know the size of arrays)
} ZBuffer;

// Function prototypes
ZBuffer* initZBuffer(const Camera* camera);
void destroyZBuffer(ZBuffer* zbuffer);

#endif // ZBUFFER_H
