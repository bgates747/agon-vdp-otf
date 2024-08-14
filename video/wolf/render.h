#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"
#include "map.h"
#include "camera.h"
#include "zbuffer.h"

#ifndef RENDER_H
#define RENDER_H


// Function prototypes
void updatePanelsAndZBuffer(Map* map, ZBuffer* zbuffer, Camera* camera);
int angleToRayIndex(float angle, float cam_theta_rad, float fov, int max_rays);
float correct_fisheye(float distance, float angle_diff);
float calculate_u(fixed8_8 intersection, fixed8_8 panel_start);
fixed8_8 calculate_strip_height(float distance, float screen_dist, int screen_height);

#endif // RENDER_H
