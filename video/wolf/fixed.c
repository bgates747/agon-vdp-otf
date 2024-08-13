#include "fixed.h"

float deg256_to_rad(fixed8_8 deg256) {
    return FIXED8_8_TO_FLOAT(deg256) * 0.0245436926f;
}

fixed8_8 rad_to_deg256(float rad) {
    return FLOAT_TO_FIXED8_8(rad * 40.7436654315f);
}
