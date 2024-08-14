#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"

#ifndef CAMERA_H
#define CAMERA_H
typedef struct Camera {
    fixed8_8 x;                // Camera x position in the map
    fixed8_8 y;                // Camera y position in the map
    fixed8_8 theta;            // Camera angle (heading) in 256-degree format
    fixed8_8 fov;              // Field of view in 256-degree format
    uint16_t screen_width;     // Screen width in pixels
    uint16_t screen_height;    // Screen height in pixels
    fixed8_8 screen_dist;      // Distance from camera to screen
} Camera;

#endif // CAMERA_H
