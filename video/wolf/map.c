#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "map.h"

// Function to check if a cell is empty
bool isCellEmpty(const Map* map, int x, int y) {
    if (x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return true; // Out of bounds cells are considered empty
    }
    Cell* cell = &map->cells[y * map->width + x];
    return (cell == NULL || (cell->map_type_status & (CELL_IS_WALL | CELL_IS_DOOR)) == 0);
}

// Function to look up a texture panel based on the image index
TexPanel* lookupTexPanel(TexPanelLut* lookupTable, uint8_t img_idx) {
    for (int i = 0; i < lookupTable->num_panels; i++) {
        if (lookupTable->panels[i]->img_idx == img_idx) {
            return lookupTable->panels[i];
        }
    }
    return NULL; // Texture panel not found
}

// Function to initialize Panels based on the Map
void initializePanels(Map* map, TexPanelLut* lookupTable) {
    for (uint16_t y = 0; y < map->height; y++) {
        for (uint16_t x = 0; x < map->width; x++) {
            Cell* cell = &map->cells[y * map->width + x];
            if (cell == NULL || !(cell->map_type_status & (CELL_IS_WALL | CELL_IS_DOOR))) {
                continue;
            }
            // Check neighbors and create panels as needed
            TexPanel* texPanel = lookupTexPanel(lookupTable, cell->img_idx);
            if (texPanel == NULL) {
                continue;  // Skip if the texture panel is not found
            }

            if (isCellEmpty(map, x, y - 1)) {
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x);
                panel->y0 = INT_TO_FIXED8_8(y);
                panel->x1 = INT_TO_FIXED8_8(x + 1);
                panel->y1 = INT_TO_FIXED8_8(y);
                cell->panels[0] = panel; // North
            }
            if (isCellEmpty(map, x, y + 1)) {
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x);
                panel->y0 = INT_TO_FIXED8_8(y + 1);
                panel->x1 = INT_TO_FIXED8_8(x + 1);
                panel->y1 = INT_TO_FIXED8_8(y + 1);
                cell->panels[2] = panel; // South
            }
            if (isCellEmpty(map, x + 1, y)) {
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = INT_TO_FIXED8_8(x + 1);
                panel->y0 = INT_TO_FIXED8_8(y);
                panel->x1 = INT_TO_FIXED8_8(x + 1);
                panel->y1 = INT_TO_FIXED8_8(y + 1);
                cell->panels[1] = panel; // East
            }
            if (isCellEmpty(map, x - 1, y)) {
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
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