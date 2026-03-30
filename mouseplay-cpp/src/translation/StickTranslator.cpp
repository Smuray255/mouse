#include "StickTranslator.h"

#include "utils/MathUtils.h"

StickTranslator::StickTranslator(float sensitivity, float deadzone, float smoothing)
    : m_sensitivity(sensitivity),
      m_deadzone(MathUtils::clamp(deadzone, 0.0f, 0.99f)),
      m_smoothing(MathUtils::clamp(smoothing, 0.0f, 1.0f)),
      m_smoothedX(0.0f),
      m_smoothedY(0.0f) {}

StickOutput StickTranslator::translate(const MouseDelta& delta) {
    const float targetX = MathUtils::clamp(delta.dx * m_sensitivity, -1.0f, 1.0f);
    const float targetY = MathUtils::clamp(-delta.dy * m_sensitivity, -1.0f, 1.0f);

    m_smoothedX = MathUtils::lerp(m_smoothedX, targetX, m_smoothing);
    m_smoothedY = MathUtils::lerp(m_smoothedY, targetY, m_smoothing);

    const float deadzonedX = MathUtils::applyDeadzone(m_smoothedX, m_deadzone);
    const float deadzonedY = MathUtils::applyDeadzone(m_smoothedY, m_deadzone);

    const int rx = static_cast<int>(MathUtils::clamp(128.0f + deadzonedX * 127.0f, 0.0f, 255.0f));
    const int ry = static_cast<int>(MathUtils::clamp(128.0f + deadzonedY * 127.0f, 0.0f, 255.0f));

    return {rx, ry};
}
