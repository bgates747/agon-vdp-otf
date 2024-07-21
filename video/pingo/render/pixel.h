#pragma once

#include <stdint.h>
#include <stdlib.h>

// Define one of the available formats
// #define PINGO_PIXEL_UINT8
// #define PINGO_PIXEL_RGB565
// #define PINGO_PIXEL_RGBA8888
#define PINGO_PIXEL_BGRA8888
// #define PINGO_PIXEL_RGB888

// Formats definitions:
#ifdef PINGO_PIXEL_UINT8
typedef struct Pixel {
    uint8_t g;
} Pixel;
#define PIXELBLACK (Pixel){0}
#define PIXELWHITE (Pixel){255}
#endif

#ifdef PINGO_PIXEL_RGB565
typedef struct Pixel {
    uint8_t red : 5;
    uint8_t green : 6;
    uint8_t blue : 5;
} Pixel;
#define PIXELBLACK (Pixel){0}
#define PIXELWHITE (Pixel){255}
#endif

#ifdef PINGO_PIXEL_RGB888
typedef struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;

#define PIXELBLACK (Pixel){0, 0, 0}
#define PIXELWHITE (Pixel){255, 255, 255}
#endif

#ifdef PINGO_PIXEL_RGBA8888
typedef struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Pixel;

#define PIXELBLACK (Pixel){0, 0, 0, 255}
#define PIXELWHITE (Pixel){255, 255, 255, 255}
#endif

#ifdef PINGO_PIXEL_BGRA8888
typedef struct Pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} Pixel;

#define PIXELBLACK (Pixel){0, 0, 0, 255}
#define PIXELWHITE (Pixel){255, 255, 255, 255}
#endif

// Interface
/// @brief Generates a random pixel.
/// @return Random pixel.
extern Pixel pixelRandom();

/// @brief Converts an 8-bit grayscale value to a pixel.
/// @param g Grayscale value.
/// @return Corresponding pixel.
extern Pixel pixelFromUInt8(uint8_t g);

/// @brief Converts a pixel to an 8-bit grayscale value.
/// @param p Pointer to the pixel.
/// @return Grayscale value.
extern uint8_t pixelToUInt8(Pixel *p);

/// @brief Creates a pixel from RGBA values.
/// @param r Red component.
/// @param g Green component.
/// @param b Blue component.
/// @param a Alpha component.
/// @return Corresponding pixel.
extern Pixel pixelFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/// @brief Multiplies a pixel by a floating-point value.
/// @param p Pixel to multiply.
/// @param f Floating-point value to multiply by.
/// @return Resulting pixel.
extern Pixel pixelMul(Pixel p, float f);
