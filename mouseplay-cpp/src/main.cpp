#include <windows.h>

#include "controller/DS4Device.h"
#include "input/InputManager.h"
#include "translation/StickTranslator.h"

namespace {
constexpr wchar_t kWindowClassName[] = L"MouseplayCppWindowClass";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_INPUT) {
        auto* inputManager = reinterpret_cast<InputManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (inputManager != nullptr) {
            inputManager->processRawInput(lParam);
        }
        return 0;
    }

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND createMessageWindow(HINSTANCE instance) {
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = kWindowClassName;

    if (RegisterClassEx(&wc) == 0) {
        return nullptr;
    }

    return CreateWindowEx(
        0,
        kWindowClassName,
        L"mouseplay-cpp",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        instance,
        nullptr);
}
}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    InputManager inputManager;
    StickTranslator translator(0.0125f, 0.05f, 0.35f);
    DS4Device ds4;

    HWND window = createMessageWindow(instance);
    if (window == nullptr) {
        return 1;
    }

    SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&inputManager));

    if (!inputManager.init(window)) {
        DestroyWindow(window);
        return 1;
    }

    if (!ds4.open()) {
        DestroyWindow(window);
        return 1;
    }

    MSG msg{};
    bool running = true;

    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) {
            break;
        }

        const MouseDelta delta = inputManager.getMouseDelta();
        const StickOutput rightStick = translator.translate(delta);

        if (!ds4.sendStickState(128, 128, static_cast<unsigned char>(rightStick.rx), static_cast<unsigned char>(rightStick.ry))) {
            running = false;
            break;
        }

        Sleep(1);
    }

    ds4.close();
    DestroyWindow(window);
    return 0;
}
