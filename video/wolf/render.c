#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "render.h"
#include "map.h"
#include "camera.h"

// Convert an angle in radians to a ray index
int angleToRayIndex(float angle, float cam_angle, float fov, int max_rays) {
    float relative_angle = angle - cam_angle;
    while (relative_angle < -M_PI) relative_angle += 2 * M_PI;
    while (relative_angle > M_PI) relative_angle -= 2 * M_PI;

    float half_fov = fov / 2.0;
    if (relative_angle < -half_fov || relative_angle > half_fov) {
        return -1; // Out of FOV
    }

    int ray_index = (int)((relative_angle + half_fov) / fov * max_rays);
    return ray_index;
}

// Correct the fisheye effect by adjusting the distance with the cosine of the angle difference
float correct_fisheye(float distance, float angle_diff) {
    return distance * cos(angle_diff);
}

// Calculate the U component of the texture UV coordinates
float calculate_u(float intersection, float panel_start) {
    return intersection - panel_start;
}

// Calculate the height of the texture strip

float calculate_strip_height(float distance, float screen_dist, int screen_height) {
    return (screen_dist / distance) * screen_height;
}

void updatePanelsAndZBuffer(Map* map, ZBuffer* zbuffer, Camera* camera) {
    printf("updatePanelsAndZBuffer: Starting update\n");

    if (!map) {
        printf("updatePanelsAndZBuffer: Invalid map object\n");
    } else {
        printf("updatePanelsAndZBuffer: Map object is valid - width=%u, height=%u\n", map->width, map->height);
    }

    if (!zbuffer) {
        printf("updatePanelsAndZBuffer: Invalid zbuffer object\n");
    } else {
        printf("updatePanelsAndZBuffer: ZBuffer object is valid.\n");
    }

    if (!camera) {
        printf("updatePanelsAndZBuffer: Invalid camera object\n");
    } else {
        printf("updatePanelsAndZBuffer: Camera object is valid - x=%f, y=%f, theta=%f, fov=%f, screen_width=%u, screen_height=%u, screen_dist=%f\n",
               camera->x, camera->y, camera->theta, camera->fov, camera->screen_width, camera->screen_height, camera->screen_dist);
    }

    if (!map || !zbuffer || !camera) {
        return;
    }

    int num_panels_processed = 0;
    float cam_angle = camera->theta;
    float half_fov = camera->fov / 2.0;

    // Set zbuffer depths to infinity
    for (int i = 0; i < camera->screen_width; i++) {
        zbuffer->depths[i] = INFINITY;
    }

    for (uint16_t x = 0; x < map->width; x++) {
        for (uint16_t y = 0; y < map->height; y++) {
            Cell* cell = &map->cells[y * map->width + x];
            if (!cell) {
                continue;
            }

            for (int i = 0; i < cell->num_panels; i++) {
                Panel* panel = cell->panels[i];
                if (!panel) {
                    continue;
                }

                num_panels_processed++;

                // Calculate relative position and distance
                float dx = panel->x0 - camera->x;
                float dy = panel->y0 - camera->y;
                float r = sqrt(dx * dx + dy * dy);
                float theta = atan2(dy, dx);

                // Determine the range of relevant rays using the angles of both edges
                int ray_start = angleToRayIndex(theta - half_fov, cam_angle, camera->fov, camera->screen_width);
                int ray_end = angleToRayIndex(theta + half_fov, cam_angle, camera->fov, camera->screen_width);

                // Ensure ray_start is less than ray_end
                if (ray_start > ray_end) {
                    int temp = ray_start;
                    ray_start = ray_end;
                    ray_end = temp;
                }

                // Iterate over the segment of the panel within the camera's FOV
                for (int ray = ray_start; ray <= ray_end; ray++) {
                    if (ray < 0 || ray >= camera->screen_width) {
                        continue;
                    }

                    float ray_angle = cam_angle - half_fov + (float)ray / (float)camera->screen_width * camera->fov;
                    float ray_cos = cos(ray_angle);
                    float ray_sin = sin(ray_angle);

                    float intersection_x = panel->x0 + r * ray_cos;
                    float intersection_y = panel->y0 + r * ray_sin;
                    float intersection_distance = sqrt((intersection_x - camera->x) * (intersection_x - camera->x) + (intersection_y - camera->y) * (intersection_y - camera->y));

                    // Correct the fisheye effect
                    float angle_diff = ray_angle - cam_angle;
                    intersection_distance = correct_fisheye(intersection_distance, angle_diff);

                    // Update z-buffer if this intersection is closer
                    if (intersection_distance < zbuffer->depths[ray]) {
                        zbuffer->depths[ray] = intersection_distance;
                        zbuffer->texture_id[ray] = panel->texture_id;

                        // Calculate U component of texture UV coordinates
                        if (panel->x0 == panel->x1) {
                            // Vertical panel
                            zbuffer->u[ray] = calculate_u(intersection_y, panel->y0);
                        } else {
                            // Horizontal panel
                            zbuffer->u[ray] = calculate_u(intersection_x, panel->x0);
                        }

                        zbuffer->strip_height[ray] = calculate_strip_height(intersection_distance, camera->screen_dist, camera->screen_height);
                    }
                }
            }
        }
    }

    printf("updatePanelsAndZBuffer: Update completed\n");
    printf("Number of panels processed: %d\n", num_panels_processed);

    printf("Ray\tDepth\tTexture ID\tU\tStrip Height\n");
    for (int i = 0; i < camera->screen_width; i++) {
        printf("%d\t%f\t%u\t%f\t%f\n", i, zbuffer->depths[i], zbuffer->texture_id[i], zbuffer->u[i], zbuffer->strip_height[i]);
    }
}
