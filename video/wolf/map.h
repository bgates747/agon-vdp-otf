#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fixed.h"
#include "camera.h"

#ifndef MAP_H
#define MAP_H

// Bitmask values for map_type_status
#define CELL_IS_DOOR      0x80  // Bit 7: door flag
#define CELL_IS_WALL      0x40  // Bit 6: wall flag
#define CELL_IS_TRIGGER   0x20  // Bit 5: trigger flag
#define CELL_IS_BLOCKING  0x10  // Bit 4: blocking flag
#define CELL_IS_START     0x08  // Bit 3: start flag (DEPRECATED)
#define CELL_IS_TO_ROOM   0x04  // Bit 2: to room flag
// bits 1 and 0 are the render_type mask
#define RENDER_TYPE_CUBE   0x00  // 0
#define RENDER_TYPE_FLOOR  0x01  // 1
#define RENDER_TYPE_NULL   0x02  // 2
#define RENDER_TYPE_SPRITE 0x03  // 3

typedef struct Map Map;
typedef struct Cell Cell;
typedef struct Panel Panel;
typedef struct TexPanel TexPanel;
typedef struct TexPanelLut TexPanelLut;

typedef struct Map {
    Cell *cells; // Single pointer for dynamic allocation
    TexPanelLut *tex_lut; // Pointer to lookup table
    uint16_t width;  // Store map width
    uint16_t height; // Store map height
} Map;

typedef struct Cell {
    uint8_t obj_id;            // Object ID
    uint8_t img_idx;           // Image Index
    uint8_t map_type_status;   // Map Type and Status Bit Mask
    uint8_t sprite_id;         // Sprite ID
    uint8_t num_panels;        // Number of panels
    Panel** panels;            // Dynamic array of pointers to panels
} Cell;

typedef struct Panel {
    float x0, y0;           // Map position of the left edge
    float x1, y1;           // Map position of the right edge
    float orientation;      // Orientation of the panel in radians
    uint16_t texture_id;    // Texture ID for the panel
    Cell* parent;           // Pointer to parent Cell
} Panel;

typedef struct TexPanel {
    uint8_t img_idx;           // Image Index
    uint16_t texture_id;       // Texture ID for the panel
    uint16_t width;            // Texture width
    uint16_t height;           // Texture height
} TexPanel;

typedef struct TexPanelLut {
    TexPanel **panels;         // Dynamic array of pointers to TexPanel
    uint8_t num_panels;        // number of elements in the lookup table
} TexPanelLut;

// Function prototypes
bool isCellEmpty(const Map* map, int x, int y);
TexPanel* lookupTexPanel(TexPanelLut* lookupTable, uint8_t img_idx);
void initializePanels(Map* map, TexPanelLut* lookupTable);
void updatePanelsAndZBuffer(Map* map, ZBuffer* zbuffer, Camera* camera);

#endif // MAP_H
