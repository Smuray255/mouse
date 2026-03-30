#include "InputManager.h"

#include <vector>

bool InputManager::init(HWND hwnd) {
    RAWINPUTDEVICE rid{};
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;

    return RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE;
}

void InputManager::processRawInput(LPARAM lParam) {
    UINT dataSize = 0;
    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER)) != 0) {
        return;
    }

    if (dataSize == 0) {
        return;
    }

    std::vector<BYTE> buffer(dataSize);
    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer.data(), &dataSize, sizeof(RAWINPUTHEADER)) != dataSize) {
        return;
    }

    const RAWINPUT* raw = reinterpret_cast<const RAWINPUT*>(buffer.data());
    if (raw->header.dwType != RIM_TYPEMOUSE) {
        return;
    }

    m_accumulatedDelta.dx += raw->data.mouse.lLastX;
    m_accumulatedDelta.dy += raw->data.mouse.lLastY;
}

MouseDelta InputManager::getMouseDelta() {
    MouseDelta out = m_accumulatedDelta;
    m_accumulatedDelta = {0, 0};
    return out;
}
