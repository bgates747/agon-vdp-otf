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

// Update Panels and ZBuffer
void updatePanelsAndZBuffer(Map* map, ZBuffer* zbuffer, Camera* camera) {
    printf("updatePanelsAndZBuffer: Starting update\n");
    float cam_angle = camera->theta;
    float half_fov = camera->fov / 2.0;

    for (uint16_t x = 0; x < map->width; x++) {
        for (uint16_t y = 0; y < map->height; y++) {
            Cell* cell = &map->cells[y * map->width + x];
            if (!cell) {
                continue;
            }

            printf("Processing cell at (%u, %u)\n", x, y);
            printf("Cell details: obj_id=%d, img_idx=%d, map_type_status=%d, sprite_id=%d\n",
                   cell->obj_id, cell->img_idx, cell->map_type_status, cell->sprite_id);

            for (int i = 0; i < 4; i++) {
                Panel* panel = cell->panels[i];
                if (!panel) {
                    continue;
                }

                printf("Processing panel %d of cell at (%u, %u)\n", i, x, y);
                printf("Panel details: x0=%f, y0=%f, x1=%f, y1=%f, texture_id=%d\n",
                       panel->x0, panel->y0, panel->x1, panel->y1, panel->texture_id);

                if (panel->parent) {
                    printf("Panel parent details: obj_id=%d, img_idx=%d, map_type_status=%d, sprite_id=%d\n",
                           panel->parent->obj_id, panel->parent->img_idx, panel->parent->map_type_status, panel->parent->sprite_id);
                } else {
                    printf("Panel parent is NULL\n");
                }

                // Calculate relative position and distance
                float dx = panel->x0 - camera->x;
                float dy = panel->y0 - camera->y;
                float r = sqrt(dx * dx + dy * dy);
                float theta = atan2(dy, dx);

                // Determine the range of relevant rays using the angles of both edges
                int ray_start = angleToRayIndex(theta - half_fov, cam_angle, camera->fov, zbuffer->screen_width);
                int ray_end = angleToRayIndex(theta + half_fov, cam_angle, camera->fov, zbuffer->screen_width);

                // Ensure ray_start is less than ray_end
                if (ray_start > ray_end) {
                    int temp = ray_start;
                    ray_start = ray_end;
                    ray_end = temp;
                }

                printf("Ray range for panel %d: start=%d, end=%d\n", i, ray_start, ray_end);
                // Iterate over the segment of the panel within the camera's FOV
                for (int ray = ray_start; ray <= ray_end; ray++) {
                    if (ray < 0 || ray >= zbuffer->screen_width) {
                        printf("Skipping ray %d: out of bounds\n", ray);
                        continue;
                    }

                    float ray_angle = cam_angle - half_fov + (float)ray / (float)zbuffer->screen_width * camera->fov;
                    float ray_cos = cos(ray_angle);
                    float ray_sin = sin(ray_angle);

                    float intersection_x = panel->x0 + r * ray_cos;
                    float intersection_y = panel->y0 + r * ray_sin;
                    float intersection_distance = sqrt((intersection_x - camera->x) * (intersection_x - camera->x) + (intersection_y - camera->y) * (intersection_y - camera->y));

                    // Correct the fisheye effect
                    float angle_diff = ray_angle - cam_angle;
                    intersection_distance = correct_fisheye(intersection_distance, angle_diff);

                    printf("Ray %d: intersection_distance=%f\n", ray, intersection_distance);

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
                        printf("Updated z-buffer for ray %d\n", ray);
                    }
                }
            }
        }
    }
    printf("updatePanelsAndZBuffer: Update completed\n");
}
