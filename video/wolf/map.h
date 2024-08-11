#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"

#ifndef MAP_H
#define MAP_H

#define MAP_SIZE_X 16  
#define MAP_SIZE_Y 16  

// Bitmask values for map_type_status
#define CELL_IS_DOOR      0x80  // Bit 7: door flag
#define CELL_IS_WALL      0x40  // Bit 6: wall flag
#define CELL_IS_TRIGGER   0x20  // Bit 5: trigger flag
#define CELL_IS_BLOCKING  0x10  // Bit 4: blocking flag
#define CELL_IS_START     0x08  // Bit 3: start flag (DEPRECATED)
#define CELL_IS_TO_ROOM   0x04  // Bit 2: to room flag

typedef struct Map Map;
typedef struct Cell Cell;
typedef struct Panel Panel;
typedef struct ZBuffer ZBuffer;
typedef struct Camera Camera;

typedef struct Map {
    Cell *cells[MAP_SIZE_X][MAP_SIZE_Y];
} Map;

typedef struct Cell {
    uint8_t obj_id;            // Object ID
    uint8_t img_idx;           // Image Index
    uint8_t map_type_status;   // Map Type and Status Bit Mask
    uint8_t sprite_id;         // Sprite ID
    Panel* panels[4];          // Pointers to panels (0 = north, 1 = east, 2 = south, 3 = west)
} Cell;

typedef struct Panel {
    fixed8_8 x0, y0;           // Map position of the left edge
    fixed8_8 x1, y1;           // Map position of the right edge
    uint16_t texture_id;       // Texture ID for the panel
    Cell* parent;              // Pointer to parent Cell
} Panel;

typedef struct ZBuffer {
    fixed8_8 *depths;          // Array of depth values for each ray
    uint16_t *texture_id;      // Texture ID for the texture
    float *u;                  // U component of texture UV coordinates
    fixed8_8 *strip_height;    // Height of the texture strip at the distance of the intersection
    int screen_width;          // Screen width in pixels (to know the size of arrays)
} ZBuffer;

typedef struct Camera {
    fixed8_8 x;                // Camera x position in the map
    fixed8_8 y;                // Camera y position in the map
    fixed8_8 theta;            // Camera angle (bearing) in 256-degree format
    float fov;                 // Field of view in radians
    int screen_width;          // Screen width in pixels
    int screen_height;         // Screen height in pixels
    float screen_dist;         // Distance from camera to screen
} Camera;

typedef struct TexPanel {
    uint8_t img_idx;           // Image Index
    uint16_t texture_id;       // Texture ID for the panel
    uint16_t width;            // Texture width
    uint16_t height;           // Texture height
} TexPanel;

typedef struct {
    TexPanel *panels[MAP_SIZE_X * MAP_SIZE_Y];
} TexPanelLookupTable;

// Function prototypes
void initLookupTable(TexPanelLookupTable *lookupTable);
void insertTexPanel(TexPanelLookupTable *lookupTable, uint8_t img_idx, uint16_t texture_id, uint16_t width, uint16_t height);
TexPanel* lookupTexPanel(TexPanelLookupTable *lookupTable, uint8_t img_idx);
bool isCellEmpty(const Map* map, int x, int y);
void initializePanels(Map* map, TexPanelLookupTable* lookupTable);
ZBuffer* initZBuffer(const Camera* camera);
void destroyZBuffer(ZBuffer* zbuffer);
void updatePanelsAndZBuffer(Map* map, ZBuffer* zbuffer, Camera* camera);
int angleToRayIndex(float angle, float cam_theta_rad, float fov, int max_rays);
float correct_fisheye(float distance, float angle_diff);
float calculate_u(fixed8_8 intersection, fixed8_8 panel_start);
fixed8_8 calculate_strip_height(float distance, float screen_dist, int screen_height);

#endif // MAP_H
