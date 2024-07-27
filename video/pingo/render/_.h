#pragma once

#include "../math/vec3.h"

#if defined(ESP_PLATFORM)
#include "esp_timer.h"

extern long long timeInMilliseconds(void);

#else
#include <sys/time.h>

extern long long timeInMilliseconds(void);

#endif

extern const Vec3f DIFFUSE_LIGHT;