# mouseplay-cpp

`mouseplay-cpp` is a Windows-only C++17 foundation for converting raw mouse movement into DualShock 4 right-stick input for PS Remote Play workflows.

It uses:
- WinAPI Raw Input (`WM_INPUT`) for true mouse delta capture
- `hidapi` over USB for direct DS4 report writes

This project intentionally does **not** use ViGEm or any virtual controller layer.

## Requirements

- Windows 10/11
- Visual Studio 2019 or newer (with C++ workload)
- CMake 3.16+
- `hidapi` headers and library available locally
- Wired DualShock 4 connected over USB

## Build (Visual Studio generator)

From the `mouseplay-cpp` directory:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 \
  -DHIDAPI_INCLUDE_DIR="C:/path/to/hidapi/include" \
  -DHIDAPI_LIBRARY="C:/path/to/hidapi/lib/hidapi.lib"
cmake --build build --config Release
```

If your `hidapi` install is under `external/hidapi`, CMake may resolve it automatically.

## Run

Build output:
- `build/Release/mouseplay-cpp.exe`

Run with a wired DS4 connected. The app creates a hidden message window to receive raw input and continuously sends stick state updates.

## Notes

- Mouse movement maps to the **right analog stick**.
- Left stick is held neutral (`128,128`) in this foundation.
- If the DS4 is unplugged or write fails, the application exits.
