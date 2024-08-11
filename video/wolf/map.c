#include "map.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void initLookupTable(TexPanelLookupTable *lookupTable) {
    for (int i = 0; i < MAP_SIZE_X * MAP_SIZE_Y; i++) {
        lookupTable->panels[i] = NULL;
    }
}

void insertTexPanel(TexPanelLookupTable *lookupTable, uint8_t img_idx, uint16_t texture_id, uint16_t width, uint16_t height) {
    TexPanel *newPanel = (TexPanel *)malloc(sizeof(TexPanel));
    newPanel->img_idx = img_idx;
    newPanel->texture_id = texture_id;
    newPanel->width = width;
    newPanel->height = height;
    lookupTable->panels[img_idx] = newPanel;
}

TexPanel* lookupTexPanel(TexPanelLookupTable *lookupTable, uint8_t img_idx) {
    return lookupTable->panels[img_idx];
}

// Function to check if a cell is empty
bool isCellEmpty(const Map* map, int x, int y) {
    if (x < 0 || x >= MAP_SIZE_X || y < 0 || y >= MAP_SIZE_Y) {
        return true; // Out of bounds cells are considered empty
    }
    Cell* cell = map->cells[x][y];
    return (cell == NULL || (cell->map_type_status & (CELL_IS_WALL | CELL_IS_DOOR)) == 0);
}

// Function to initialize Panels based on the Map
void initializePanels(Map* map, TexPanelLookupTable* lookupTable) {
    for (int x = 0; x < MAP_SIZE_X; x++) {
        for (int y = 0; y < MAP_SIZE_Y; y++) {
            Cell* cell = map->cells[x][y];
            if (cell == NULL || !(cell->map_type_status & (CELL_IS_WALL | CELL_IS_DOOR))) {
                continue;
            }
            // Check neighbors and create panels as needed
            TexPanel* texPanel = lookupTexPanel(lookupTable, cell->img_idx);
            if (texPanel == NULL) {
                continue;  // Skip if the texture panel is not found
            }

            if (isCellEmpty(map, x, y - 1)) {
                Panel* panel = malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x);
                panel->y0 = INT_TO_FIXED8_8(y);
                panel->x1 = INT_TO_FIXED8_8(x + 1);
                panel->y1 = INT_TO_FIXED8_8(y);
                cell->panels[0] = panel; // North
            }
            if (isCellEmpty(map, x, y + 1)) {
                Panel* panel = malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x);
                panel->y0 = INT_TO_FIXED8_8(y + 1);
                panel->x1 = INT_TO_FIXED8_8(x + 1);
                panel->y1 = INT_TO_FIXED8_8(y + 1);
                cell->panels[2] = panel; // South
            }
            if (isCellEmpty(map, x + 1, y)) {
                Panel* panel = malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x + 1);
                panel->y0 = INT_TO_FIXED8_8(y);
                panel->x1 = INT_TO_FIXED8_8(x + 1);
                panel->y1 = INT_TO_FIXED8_8(y + 1);
                cell->panels[1] = panel; // East
            }
            if (isCellEmpty(map, x - 1, y)) {
                Panel* panel = malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x);
                panel->y0 = INT_TO_FIXED8_8(y);
                panel->x1 = INT_TO_FIXED8_8(x);
                panel->y1 = INT_TO_FIXED8_8(y + 1);
                cell->panels[3] = panel; // West
            }
        }
    }
}

// Constructor for ZBuffer
ZBuffer* initZBuffer(const Camera* camera) {
    ZBuffer* zbuffer = (ZBuffer*)malloc(sizeof(ZBuffer));
    if (zbuffer == NULL) {
        return NULL; // Memory allocation failed
    }

    zbuffer->screen_width = camera->screen_width;

    zbuffer->depths = (fixed8_8*)malloc(zbuffer->screen_width * sizeof(fixed8_8));
    zbuffer->texture_id = (uint16_t*)malloc(zbuffer->screen_width * sizeof(uint16_t));
    zbuffer->u = (float*)malloc(zbuffer->screen_width * sizeof(float));
    zbuffer->strip_height = (fixed8_8*)malloc(zbuffer->screen_width * sizeof(fixed8_8));

    if (zbuffer->depths == NULL || zbuffer->texture_id == NULL || zbuffer->u == NULL || zbuffer->strip_height == NULL) {
        free(zbuffer->depths);
        free(zbuffer->texture_id);
        free(zbuffer->u);
        free(zbuffer->strip_height);
        free(zbuffer);
        return NULL; // Memory allocation failed
    }

    // Initialize arrays to default values (if needed)
    memset(zbuffer->depths, 0, zbuffer->screen_width * sizeof(fixed8_8));
    memset(zbuffer->texture_id, 0, zbuffer->screen_width * sizeof(uint16_t));
    memset(zbuffer->u, 0, zbuffer->screen_width * sizeof(float));
    memset(zbuffer->strip_height, 0, zbuffer->screen_width * sizeof(fixed8_8));

    return zbuffer;
}

// Convert an angle in radians to a ray index
int angleToRayIndex(float angle, float cam_theta_rad, float fov, int max_rays) {
    float relative_angle = angle - cam_theta_rad;
    while (relative_angle < -M_PI) relative_angle += 2 * M_PI;
    while (relative_angle > M_PI) relative_angle -= 2 * M_PI;

    float half_fov = fov / 2.0;
    if (relative_angle < -half_fov || relative_angle > half_fov) {
        return -1; // Out of FOV
    }

    int ray_index = (int)((relative_angle + half_fov) / fov * max_rays);
    return ray_index;
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

// Correct the fisheye effect by adjusting the distance with the cosine of the angle difference
float correct_fisheye(float distance, float angle_diff) {
    return distance * cos(angle_diff);
}

// Calculate the U component of the texture UV coordinates
float calculate_u(fixed8_8 intersection, fixed8_8 panel_start) {
    return FIXED8_8_TO_FLOAT(intersection - panel_start);
}

// Calculate the height of the texture strip
fixed8_8 calculate_strip_height(float distance, float screen_dist, int screen_height) {
    float height = (screen_dist / distance) * screen_height;
    return FLOAT_TO_FIXED8_8(height);
}

// Update Panels and ZBuffer
void updatePanelsAndZBuffer(Map* map, ZBuffer* zbuffer, Camera* camera) {
    initZBuffer(camera);
    float cam_theta_rad = FIXED8_8_TO_FLOAT(camera->theta) * (M_PI / 128.0); // Convert to radians
    float half_fov = camera->fov / 2.0;

    for (int x = 0; x < MAP_SIZE_X; x++) {
        for (int y = 0; y < MAP_SIZE_Y; y++) {
            Cell* cell = map->cells[x][y];
            if (cell == NULL) {
                continue;
            }

            for (int i = 0; i < 4; i++) {
                Panel* panel = cell->panels[i];
                if (panel == NULL) {
                    continue;
                }

                // Calculate relative position and distance
                fixed8_8 dx = panel->x0 - camera->x;
                fixed8_8 dy = panel->y0 - camera->y;
                float r = sqrt((float)(FIXED8_8_TO_FLOAT(dx) * FIXED8_8_TO_FLOAT(dx) + FIXED8_8_TO_FLOAT(dy) * FIXED8_8_TO_FLOAT(dy)));
                float theta = atan2(FIXED8_8_TO_FLOAT(dy), FIXED8_8_TO_FLOAT(dx)) * (128.0 / M_PI); // Convert to fixed 8.8 format

                // Convert theta to radians
                float theta_rad = theta * (M_PI / 128.0);

                // Determine the range of relevant rays using the angles of both edges
                int ray_start = angleToRayIndex(theta_rad - half_fov, cam_theta_rad, camera->fov, zbuffer->screen_width);
                int ray_end = angleToRayIndex(theta_rad + half_fov, cam_theta_rad, camera->fov, zbuffer->screen_width);

                // Ensure ray_start is less than ray_end
                if (ray_start > ray_end) {
                    int temp = ray_start;
                    ray_start = ray_end;
                    ray_end = temp;
                }

                // Iterate over the segment of the panel within the camera's FOV
                for (int ray = ray_start; ray <= ray_end; ray++) {
                    float ray_angle = cam_theta_rad - half_fov + (float)ray / (float)zbuffer->screen_width * camera->fov;
                    float ray_cos = cos(ray_angle);
                    float ray_sin = sin(ray_angle);

                    float intersection_x = FIXED8_8_TO_FLOAT(panel->x0) + r * ray_cos;
                    float intersection_y = FIXED8_8_TO_FLOAT(panel->y0) + r * ray_sin;
                    float intersection_distance = sqrt((intersection_x - FIXED8_8_TO_FLOAT(camera->x)) * (intersection_x - FIXED8_8_TO_FLOAT(camera->x)) + (intersection_y - FIXED8_8_TO_FLOAT(camera->y)) * (intersection_y - FIXED8_8_TO_FLOAT(camera->y)));

                    // Correct the fisheye effect
                    float angle_diff = ray_angle - cam_theta_rad;
                    intersection_distance = correct_fisheye(intersection_distance, angle_diff);

                    // Update z-buffer if this intersection is closer
                    if (FLOAT_TO_FIXED8_8(intersection_distance) < zbuffer->depths[ray]) {
                        zbuffer->depths[ray] = FLOAT_TO_FIXED8_8(intersection_distance);
                        zbuffer->texture_id[ray] = panel->texture_id;

                        // Calculate U component of texture UV coordinates
                        if (panel->x0 == panel->x1) {
                            // Vertical panel
                            zbuffer->u[ray] = calculate_u(FLOAT_TO_FIXED8_8(intersection_y), panel->y0);
                        } else {
                            // Horizontal panel
                            zbuffer->u[ray] = calculate_u(FLOAT_TO_FIXED8_8(intersection_x), panel->x0);
                        }

                        zbuffer->strip_height[ray] = calculate_strip_height(intersection_distance, camera->screen_dist, camera->screen_height);
                    }
                }
            }
        }
    }
}
