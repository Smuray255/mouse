#include "MathUtils.h"

#include <cmath>

namespace MathUtils {

float lerp(float from, float to, float alpha) {
    return from + (to - from) * alpha;
}

float applyDeadzone(float value, float deadzone) {
    const float magnitude = std::fabs(value);
    if (magnitude <= deadzone) {
        return 0.0f;
    }

    const float sign = value < 0.0f ? -1.0f : 1.0f;
    const float normalized = (magnitude - deadzone) / (1.0f - deadzone);
    return sign * clamp(normalized, 0.0f, 1.0f);
}

}  // namespace MathUtils
