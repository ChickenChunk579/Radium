cmake_minimum_required(VERSION 3.12)
project(Radium)

set(BUILD_SHARED_LIBS OFF)
set(SDL2TTF_VENDORED ON)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

add_subdirectory(subprojects/entt)
add_subdirectory(subprojects/Rune)
add_subdirectory(subprojects/SDL)
add_subdirectory(subprojects/SDL_ttf)
add_subdirectory(subprojects/SDL_image)
add_subdirectory(subprojects/spdlog)
add_subdirectory(subprojects/tracy)

add_library(imgui STATIC
    subprojects/imgui/imgui_demo.cpp
    subprojects/imgui/imgui_draw.cpp
    subprojects/imgui/imgui_tables.cpp
    subprojects/imgui/imgui_widgets.cpp
    subprojects/imgui/imgui.cpp
    subprojects/imgui/backends/imgui_impl_sdl2.cpp
)

target_link_libraries(imgui PUBLIC SDL2::SDL2)

target_include_directories(imgui PUBLIC subprojects/imgui)

add_library(Radium STATIC
    src/Radium/Application.cpp
    src/Radium/FontRenderer.cpp
    src/Radium/Math.cpp
    src/Radium/SpriteBatchRegistry.cpp
    src/Radium/imgui_impl_rune.cpp
)

target_link_libraries(Radium PUBLIC
    SDL2::SDL2
    SDL2_image
    SDL2_ttf
    spdlog::spdlog
    EnTT::EnTT
    Rune
    imgui
)
target_include_directories(Radium PUBLIC
    src
    subprojects/RmlUi/Include
)

target_link_libraries(Radium PRIVATE TracyClient)
target_compile_definitions(Radium PRIVATE TRACY_ENABLE)

if (EMSCRIPTEN)
    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGPU=1 -sASYNCIFY -s WASM=1 -g4 -s ASSERTIONS=1")

    # Generate .html output
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

if (NOT ANDROID)
add_executable(TestApp
    src/TestApp/main.cpp
)
else()
add_library(TestApp SHARED
    src/TestApp/main.cpp
)
endif()

target_link_libraries(TestApp PUBLIC Radium)


if (EMSCRIPTEN)
    set_target_properties(TestApp PROPERTIES
        LINK_FLAGS "--preload-file assets"
    )
endif ()

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/TestApp/assets")
set(ASSETS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

add_custom_command(
    OUTPUT ${ASSETS_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_OUTPUT_DIR}
    COMMENT "Copying assets to build output directory"
    DEPENDS ${ASSETS_SOURCE_DIR}
)

add_custom_target(copy_assets ALL DEPENDS ${ASSETS_OUTPUT_DIR})
add_dependencies(TestApp copy_assets)
