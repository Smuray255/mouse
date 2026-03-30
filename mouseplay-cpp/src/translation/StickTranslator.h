#pragma once

#include "input/InputManager.h"

struct StickOutput {
    int rx;
    int ry;
};

class StickTranslator {
public:
    StickTranslator(float sensitivity, float deadzone, float smoothing);
    StickOutput translate(const MouseDelta& delta);

private:
    float m_sensitivity;
    float m_deadzone;
    float m_smoothing;
    float m_smoothedX;
    float m_smoothedY;
};
