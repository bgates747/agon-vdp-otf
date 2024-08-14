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
    printf("\ninitializePanels: Starting initialization\n");
    if (lookupTable == NULL) {
        printf("Lookup table is NULL\n");
    }

    printf("Lookup table characteristics:\n");
    printf("  - Number of panels: %d\n", lookupTable->num_panels);
    // printf("  - Panels:\n");
    // for (int i = 0; i < lookupTable->num_panels; i++) {
    //     TexPanel* panel = lookupTable->panels[i];
    //     printf("    - Panel %d:\n", i);
    //     printf("      - Image index: %d\n", panel->img_idx);
    //     printf("      - Texture ID: %d\n", panel->texture_id);
    //     printf("      - Width: %d\n", panel->width);
    //     printf("      - Height: %d\n", panel->height);
    // }

    for (uint16_t y = 0; y < map->height; y++) {
        for (uint16_t x = 0; x < map->width; x++) {
            // printf("Accessing cell at (%u, %u)\n", x, y);
            Cell* cell = &map->cells[y * map->width + x];
            if (cell == NULL) {
                printf("Cell at (%u, %u) is NULL\n", x, y);
                continue;
            }

            // printf("Processing cell at (%u, %u) - obj_id=%d, img_idx=%d, map_type_status=%d, sprite_id=%d\n", x, y, cell->obj_id, cell->img_idx, cell->map_type_status, cell->sprite_id);

            if ((cell->map_type_status & 0x03) != RENDER_TYPE_CUBE) {
                // printf("Cell at (%u, %u) is not a cube\n", x, y);
                cell->num_panels = 0;
                continue;
            }

            if (!(cell->map_type_status & (CELL_IS_WALL | CELL_IS_DOOR))) {
                // printf("Cell at (%u, %u) is not a wall or door\n", x, y);
                cell->num_panels = 0;
                continue;
            }

            // Check neighbors and create panels as needed
            // printf("Looking up TexPanel for img_idx=%d\n", cell->img_idx);
            TexPanel* texPanel = lookupTexPanel(lookupTable, cell->img_idx);
            if (texPanel == NULL) {
                printf("TexPanel not found for img_idx=%d\n", cell->img_idx);
                continue;  // Skip if the texture panel is not found
            }

            // printf("TexPanel found: texture_id=%d, width=%d, height=%d\n", texPanel->texture_id, texPanel->width, texPanel->height);

            // printf("Allocating memory for panels array\n");
            cell->panels = (Panel **)malloc(4 * sizeof(Panel *));
            cell->num_panels = 0;

            // printf("Checking if cell (%u, %u) has a North neighbor\n", x, y);
            if (isCellEmpty(map, x, y - 1)) {
                // printf("Creating North panel for cell at (%u, %u)\n", x, y);
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                if (panel == NULL) {
                    printf("Failed to allocate memory for North panel at (%u, %u)\n", x, y);
                    continue;
                }
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = x;
                panel->y0 = y;
                panel->x1 = x + 1;
                panel->y1 = y;
                panel->orientation = 0.0f; // North is 0 radians
                cell->panels[cell->num_panels++] = panel; // North
                // printf("North panel created: x0=%f, y0=%f, x1=%f, y1=%f, texture_id=%d\n", panel->x0, panel->y0, panel->x1, panel->y1, panel->texture_id);
            }

            // printf("Checking if cell (%u, %u) has a South neighbor\n", x, y);
            if (isCellEmpty(map, x, y + 1)) {
                // printf("Creating South panel for cell at (%u, %u)\n", x, y);
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                if (panel == NULL) {
                    printf("Failed to allocate memory for South panel at (%u, %u)\n", x, y);
                    continue;
                }
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = x;
                panel->y0 = y + 1;
                panel->x1 = x + 1;
                panel->y1 = y + 1;
                panel->orientation = M_PI; // South is 180 degrees (pi radians)
                cell->panels[cell->num_panels++] = panel; // South
                // printf("South panel created: x0=%f, y0=%f, x1=%f, y1=%f, texture_id=%d\n", panel->x0, panel->y0, panel->x1, panel->y1, panel->texture_id);
            }

            // printf("Checking if cell (%u, %u) has an East neighbor\n", x, y);
            if (isCellEmpty(map, x + 1, y)) {
                // printf("Creating East panel for cell at (%u, %u)\n", x, y);
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                if (panel == NULL) {
                    printf("Failed to allocate memory for East panel at (%u, %u)\n", x, y);
                    continue;
                }
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = x + 1;
                panel->y0 = y;
                panel->x1 = x + 1;
                panel->y1 = y + 1;
                panel->orientation = M_PI_2; // East is 90 degrees (pi/2 radians)
                cell->panels[cell->num_panels++] = panel; // East
                // printf("East panel created: x0=%f, y0=%f, x1=%f, y1=%f, texture_id=%d\n", panel->x0, panel->y0, panel->x1, panel->y1, panel->texture_id);
            }

            // printf("Checking if cell (%u, %u) has a West neighbor\n", x, y);
            if (isCellEmpty(map, x - 1, y)) {
                // printf("Creating West panel for cell at (%u, %u)\n", x, y);
                Panel* panel = (Panel *)malloc(sizeof(Panel)); // Allocate memory for the panel
                if (panel == NULL) {
                    printf("Failed to allocate memory for West panel at (%u, %u)\n", x, y);
                    continue;
                }
                panel->texture_id = texPanel->texture_id;
                panel->parent = cell;
                panel->x0 = x;
                panel->y0 = y;
                panel->x1 = x;
                panel->y1 = y + 1;
                panel->orientation = -M_PI_2; // West is -90 degrees (-pi/2 radians)
                cell->panels[cell->num_panels++] = panel; // West
                // printf("West panel created: x0=%f, y0=%f, x1=%f, y1=%f, texture_id=%d\n", panel->x0, panel->y0, panel->x1, panel->y1, panel->texture_id);
            }
        }
    }

    printf("initializePanels: Initialization completed\n\n");
}
