#pragma once

namespace MathUtils {

template <typename T>
T clamp(T value, T minValue, T maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

float lerp(float from, float to, float alpha);
float applyDeadzone(float value, float deadzone);

}  // namespace MathUtils
