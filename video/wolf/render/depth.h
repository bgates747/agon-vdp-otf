#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ZBUFFER32 // [ZBUFFER32 | ZBUFFER16 | ZBUFFER8]

#ifdef ZBUFFER32
typedef struct tag_WolfDepth {
    uint32_t d;
} WolfDepth;
#endif

#ifdef ZBUFFER16
typedef struct Depth {
    uint16_t d;
} Depth;
#endif

#ifdef ZBUFFER8
typedef struct Depth {
    uint8_t d;
} Depth;
#endif

void depth_write(WolfDepth * d, int idx, float value);
bool depth_check(WolfDepth * d, int idx, float value);

