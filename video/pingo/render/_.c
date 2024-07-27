#include "_.h"

#if defined(ESP_PLATFORM)

long long timeInMilliseconds(void) {
    return esp_timer_get_time() / 1000;
}

#else

long long timeInMilliseconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

#endif

const Vec3f DIFFUSE_LIGHT = { -8, -5, 5 };