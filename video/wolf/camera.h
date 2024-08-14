#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"

#ifndef CAMERA_H
#define CAMERA_H

// Forward declarations
typedef struct ZBuffer ZBuffer;
typedef struct Camera {
    float x;                // Camera x position in the map
    float y;                // Camera y position in the map
    float theta;            // Camera angle (heading) in radians
    float fov;              // Field of view in radians
    uint16_t screen_width;  // Screen width in pixels
    uint16_t screen_height; // Screen height in pixels
    float screen_dist;      // Distance from camera to screen
    ZBuffer* zbuffer;       // Pointer to the ZBuffer
} Camera;
typedef struct ZBuffer {
    float *depths;          // Array of depth values for each ray
    uint16_t *texture_id;      // Texture ID for the texture
    float *u;                  // U component of texture UV coordinates
    float *strip_height;    // Height of the texture strip at the distance of the intersection
    int screen_width;          // Screen width in pixels (to know the size of arrays)
} ZBuffer;

// Function prototypes
ZBuffer* create_zbuffer(Camera* camera);
void destroy_zbuffer(ZBuffer* zbuffer);

#endif // CAMERA_H
