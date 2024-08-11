#pragma once
#include <stdint.h>

typedef int16_t fixed8_8;

// Convert from integer to 8.8 fixed-point
#define INT_TO_FIXED8_8(x) ((fixed8_8)((x) << 8))

// Convert from 8.8 fixed-point to integer (truncating the fractional part)
#define FIXED8_8_TO_INT(x) ((int16_t)((x) >> 8))

// Convert from floating-point to 8.8 fixed-point
#define FLOAT_TO_FIXED8_8(x) ((fixed8_8)((x) * 256.0f))

// Convert from 8.8 fixed-point to floating-point
#define FIXED8_8_TO_FLOAT(x) ((float)(x) / 256.0f)

// Add two 8.8 fixed-point numbers
#define FIXED8_8_ADD(x, y) ((x) + (y))

// Subtract two 8.8 fixed-point numbers
#define FIXED8_8_SUB(x, y) ((x) - (y))

// Multiply two 8.8 fixed-point numbers
#define FIXED8_8_MUL(x, y) ((fixed8_8)(((int32_t)(x) * (y)) >> 8))

// Divide two 8.8 fixed-point numbers
#define FIXED8_8_DIV(x, y) ((fixed8_8)(((int32_t)(x) << 8) / (y)))

typedef struct {
    fixed8_8 x, y;
} Vec2_8_8;

float deg256_to_rad(fixed8_8 deg256) {
    return FIXED8_8_TO_FLOAT(deg256) * 0.0245436926f;
}

fixed8_8 rad_to_deg256(float rad) {
    return FLOAT_TO_FIXED8_8(rad * 40.7436654315f);
}
