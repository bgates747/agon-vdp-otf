#include <stddef.h> 
#include <stdlib.h>
#include <stdio.h>

#include "camera.h"

ZBuffer* create_zbuffer(Camera* camera) {
    if (!camera) {
        printf("create_zbuffer: camera is NULL\n");
        return NULL;
    }

    printf("create_zbuffer: Allocating memory for ZBuffer\n");
    ZBuffer* zbuffer = (ZBuffer*)malloc(sizeof(ZBuffer));
    if (!zbuffer) {
        printf("create_zbuffer: Failed to allocate memory for ZBuffer struct\n");
        return NULL;
    }

    printf("create_zbuffer: Allocating memory for depths array\n");
    zbuffer->depths = (float*)malloc(camera->screen_width * sizeof(float));
    if (!zbuffer->depths) {
        printf("create_zbuffer: Failed to allocate memory for depths array\n");
        free(zbuffer);
        return NULL;
    }

    printf("create_zbuffer: Allocating memory for texture_id array\n");
    zbuffer->texture_id = (uint16_t*)malloc(camera->screen_width * sizeof(uint16_t));
    if (!zbuffer->texture_id) {
        printf("create_zbuffer: Failed to allocate memory for texture_id array\n");
        free(zbuffer->depths);
        free(zbuffer);
        return NULL;
    }

    printf("create_zbuffer: Allocating memory for u array\n");
    zbuffer->u = (float*)malloc(camera->screen_width * sizeof(float));
    if (!zbuffer->u) {
        printf("create_zbuffer: Failed to allocate memory for u array\n");
        free(zbuffer->depths);
        free(zbuffer->texture_id);
        free(zbuffer);
        return NULL;
    }

    printf("create_zbuffer: Allocating memory for strip_height array\n");
    zbuffer->strip_height = (float*)malloc(camera->screen_width * sizeof(float));
    if (!zbuffer->strip_height) {
        printf("create_zbuffer: Failed to allocate memory for strip_height array\n");
        free(zbuffer->depths);
        free(zbuffer->texture_id);
        free(zbuffer->u);
        free(zbuffer);
        return NULL;
    }

    printf("create_zbuffer: ZBuffer successfully created\n");
    return zbuffer;
}

// Function to destroy a ZBuffer
void destroy_zbuffer(ZBuffer* zbuffer) {
    if (zbuffer) {
        printf("destroy_zbuffer: Freeing depths array\n");
        free(zbuffer->depths);

        printf("destroy_zbuffer: Freeing texture_id array\n");
        free(zbuffer->texture_id);

        printf("destroy_zbuffer: Freeing u array\n");
        free(zbuffer->u);

        printf("destroy_zbuffer: Freeing strip_height array\n");
        free(zbuffer->strip_height);

        printf("destroy_zbuffer: Freeing ZBuffer struct\n");
        free(zbuffer);
    }
}
