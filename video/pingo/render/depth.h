#pragma once

#include <stdbool.h>
#include <stdint.h>

/// @brief Define the Z-buffer precision.
/// Options: [ZBUFFER32 | ZBUFFER16 | ZBUFFER8]
#define ZBUFFER32

#ifdef ZBUFFER32
/// @brief Structure representing a 32-bit depth value.
typedef struct PingoDepth {
    uint32_t d; ///< 32-bit depth value
} PingoDepth;
#endif

#ifdef ZBUFFER16
/// @brief Structure representing a 16-bit depth value.
typedef struct Depth {
    uint16_t d; ///< 16-bit depth value
} Depth;
#endif

#ifdef ZBUFFER8
/// @brief Structure representing an 8-bit depth value.
typedef struct Depth {
    uint8_t d; ///< 8-bit depth value
} Depth;
#endif

/// @brief Writes a depth value to the depth buffer.
/// @param d Pointer to the depth buffer.
/// @param idx Index at which to write the depth value.
/// @param value Depth value to write.
void depth_write(PingoDepth *d, int idx, float value);

/// @brief Checks a depth value against the depth buffer.
/// @param d Pointer to the depth buffer.
/// @param idx Index at which to check the depth value.
/// @param value Depth value to check.
/// @return true if the depth value is less than the stored depth value, false otherwise.
bool depth_check(PingoDepth *d, int idx, float value);
