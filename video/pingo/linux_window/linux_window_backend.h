
#pragma once

#include "render/backend.h"
#include "math/vec2.h"

typedef struct Pixel Pixel;

typedef struct LinuxWindowBackend {
    Backend backend;
    Vec2i size;
} LinuxWindowBackend;


void linuxWindowBackendInit(LinuxWindowBackend * linuxWindowBackend, Vec2i size);
