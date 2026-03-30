cmake_minimum_required(VERSION 3.16)
project(mouseplay_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR "mouseplay-cpp supports Windows only.")
endif()

find_path(HIDAPI_INCLUDE_DIR
    NAMES hidapi/hidapi.h
    HINTS
        ${CMAKE_SOURCE_DIR}/external/hidapi
        ${CMAKE_SOURCE_DIR}/external/hidapi/include
)

find_library(HIDAPI_LIBRARY
    NAMES hidapi hidapi.lib
    HINTS
        ${CMAKE_SOURCE_DIR}/external/hidapi
        ${CMAKE_SOURCE_DIR}/external/hidapi/lib
)

if(NOT HIDAPI_INCLUDE_DIR OR NOT HIDAPI_LIBRARY)
    message(FATAL_ERROR "hidapi not found. Set HIDAPI_INCLUDE_DIR and HIDAPI_LIBRARY or place hidapi in external/hidapi.")
endif()

add_executable(mouseplay-cpp WIN32
    src/main.cpp
    src/input/InputManager.cpp
    src/translation/StickTranslator.cpp
    src/controller/DS4Device.cpp
    src/utils/MathUtils.cpp
)

target_include_directories(mouseplay-cpp PRIVATE
    src
    ${HIDAPI_INCLUDE_DIR}
)

target_link_libraries(mouseplay-cpp PRIVATE
    ${HIDAPI_LIBRARY}
)
