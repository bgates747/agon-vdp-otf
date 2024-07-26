#pragma once

#if defined(ESP_PLATFORM)
#include "esp_timer.h"

extern long long timeInMilliseconds(void);

#else
#include <sys/time.h>

extern long long timeInMilliseconds(void);

#endif
