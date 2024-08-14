#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"

#ifndef CAMERA_H
#define CAMERA_H
typedef struct Camera {
    float x;                // Camera x position in the map
    float y;                // Camera y position in the map
    float theta;            // Camera angle (heading) in radians
    float fov;              // Field of view in radians
    uint16_t screen_width;     // Screen width in pixels
    uint16_t screen_height;    // Screen height in pixels
    float screen_dist;      // Distance from camera to screen
} Camera;

#endif // CAMERA_H
