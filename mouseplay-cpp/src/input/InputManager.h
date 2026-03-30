#pragma once

#include <windows.h>

struct MouseDelta {
    int dx;
    int dy;
};

class InputManager {
public:
    bool init(HWND hwnd);
    void processRawInput(LPARAM lParam);
    MouseDelta getMouseDelta();

private:
    MouseDelta m_accumulatedDelta{0, 0};
};
