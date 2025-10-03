cmake_minimum_required(VERSION 3.12)
project(Radium LANGUAGES C CXX)

set(BUILD_SHARED_LIBS OFF)
set(SDL2TTF_VENDORED ON)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

if (IOS)
    # Disable ImageIO for SDL2_image as it's not available for iOS, just OS X, and will fail at link time
    set(SDL2IMAGE_BACKEND_IMAGEIO 0)
endif()

add_subdirectory(subprojects/box2d)
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
    src/Radium/Input.cpp
    src/Radium/AssetLoader.cpp
    src/Radium/Nodes/Tree.cpp
    src/Radium/Nodes/Node.cpp
    src/Radium/Nodes/ClassDB.cpp
    src/Radium/Nodes/2D/Node2D.cpp
    src/Radium/Nodes/2D/Sprite2D.cpp
)

target_link_libraries(Radium PUBLIC
    SDL2::SDL2
    SDL2_image
    SDL2_ttf
    SDL2main
    spdlog::spdlog
    Rune
    imgui
    box2d
)
target_include_directories(Radium PUBLIC
    src
    subprojects/RmlUi/Include
)

target_link_libraries(Radium PRIVATE TracyClient)
target_compile_definitions(Radium PRIVATE TRACY_ENABLE)

if (EMSCRIPTEN)
    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sALLOW_MEMORY_GROWTH -s USE_WEBGPU=1 -sASYNCIFY -s WASM=1 -g4 -s ASSERTIONS=1")

    # Generate .html output
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

if (NOT ANDROID AND NOT IOS)
    add_executable(TestApp
        src/TestApp/main.cpp
    )
else()
    if (ANDROID)
        add_library(TestApp SHARED
            src/TestApp/main.cpp
        )
    else()

        # Path to your assets folder
        set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/src/TestApp/assets")

        # Get all files recursively
        file(GLOB_RECURSE ASSET_FILES
            "${ASSETS_DIR}/*"
        )

        foreach(asset ${ASSET_FILES})
            set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/TestApp")

            file(RELATIVE_PATH REL_PATH "${BASE_DIR}" "${asset}")

            message(STATUS "Asset path: ${asset}")
            message(STATUS "Relative path: ${REL_PATH}")

            # Set the MACOSX_PACKAGE_LOCATION to Resources/assets/[subdirs]
            set_source_files_properties(${asset} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/assets/${REL_PATH}"
            )
        endforeach()

        # Add executable
        add_executable(TestApp MACOSX_BUNDLE
            src/TestApp/main.cpp
            ${ASSET_FILES}
        )

        

        set_target_properties(TestApp PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.radium.TestApp"
            MACOSX_BUNDLE_BUNDLE_NAME "TestApp"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
            MACOSX_BUNDLE_INFO_STRING "My Radium App"
            RESOURCE "${ASSET_FILES}" # optional folder for assets
        )



    endif()
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



if(APPLE AND IOS)
    target_compile_definitions(Radium PRIVATE TARGET_OS_IOS=1)
endif()