cmake_minimum_required(VERSION 3.15)
project(Rune LANGUAGES C CXX)

# ─── Build Option ──────────────────────────────────────────────────────────────
option(BUILD_TESTAPP "Build the test application" OFF)

# ─── Output Folders ────────────────────────────────────────────────────────────
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ─── Detect Emscripten ─────────────────────────────────────────────────────────
if("${CMAKE_CXX_COMPILER}" MATCHES "em\\+\\+")
    set(IS_WASM TRUE)
    message(STATUS "Configuring for WebAssembly (Emscripten + emwebgpu)")
else()
    set(IS_WASM FALSE)
    message(STATUS "Configuring for native (Linux/other)")
endif()

# ─── Rune Library ─────────────────────────────────────────────────────────────
set(RUNE_SOURCES
    src/Rune/Rune.cpp
    src/Rune/SpriteBatch.cpp
    src/Rune/RuneBindings.cpp
    src/Rune/GeometryRenderer.cpp
    src/Rune/Texture.cpp
)

# Add Objective-C++ bridge on macOS
if(APPLE)
    list(APPEND RUNE_SOURCES src/Rune/objc_bridge.mm)
endif()

add_library(Rune STATIC ${RUNE_SOURCES})

target_include_directories(Rune PUBLIC
    src
)

if(IS_WASM)
    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGPU=1 -sASYNCIFY -s USE_SDL=2 -s WASM=1")
    # Generate .html output
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    # ─── Subdirectories ────────────────────────────────────────────────────────────
    add_subdirectory(subprojects/wgpu-native)

    # Native configuration
    target_link_libraries(Rune PUBLIC wgpu::native)

    # SDL2 via pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

# ─── Optional Test App ─────────────────────────────────────────────────────────
if(BUILD_TESTAPP)
    add_executable(testapp testapp/main.cpp)
    target_link_libraries(testapp PRIVATE Rune)

    if(NOT IS_WASM)
        target_include_directories(testapp PRIVATE ${SDL2_INCLUDE_DIRS} lib/include)
        target_link_libraries(testapp PRIVATE ${SDL2_LIBRARIES})
    endif()
endif()

# ─── macOS / iOS Frameworks ─────────────────────────────────────────────────────
if(APPLE)
    target_link_libraries(Rune PRIVATE "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")
    if(IOS)
        target_link_libraries(Rune PRIVATE "-framework UIKit")
    else()
        target_link_libraries(Rune PRIVATE "-framework Cocoa")
    endif()
endif()
