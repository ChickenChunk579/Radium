cmake_minimum_required(VERSION 3.12)
project(Radium LANGUAGES C CXX)

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug mode
        add_compile_options(/MTd /Zi /Od /RTC1)
        add_compile_definitions(_DEBUG _ITERATOR_DEBUG_LEVEL=2)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        # Release mode
        add_compile_options(/MT /O2)
        add_compile_definitions(NDEBUG _ITERATOR_DEBUG_LEVEL=0)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endif()

set(BUILD_SHARED_LIBS OFF)
set(SDL2TTF_VENDORED ON)
set(BUILD_TESTING OFF)
set(BUILD_SAMPLES OFF)
set(DYNLOAD_ENABLED OFF)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (IOS)
    # Disable ImageIO for SDL2_image as it's not available for iOS, just OS X, and will fail at link time
    set(SDL2IMAGE_BACKEND_IMAGEIO 0)
endif()

add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -DTRACY_NO_INVARIANT_CHECK=1)

if(ANDROID)
    link_libraries(log)
endif()

add_subdirectory(subprojects/box2d)
add_subdirectory(subprojects/tracy)
add_subdirectory(subprojects/tiny-process-library)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_definitions(-DMAKE_LIB)

add_library(imgui STATIC
    subprojects/imgui/imgui_demo.cpp
    subprojects/imgui/imgui_draw.cpp
    subprojects/imgui/imgui_tables.cpp
    subprojects/imgui/imgui_widgets.cpp
    subprojects/imgui/imgui.cpp
)

target_link_libraries(imgui PUBLIC)

target_include_directories(imgui PUBLIC subprojects/imgui)

add_library(Radium STATIC
    subprojects/ImGuiColorTextEdit/TextEditor.cpp
    subprojects/ImGuiFileDialog/ImGuiFileDialog.cpp
    src/Radium/Application.cpp
    src/Radium/Math.cpp
    src/Radium/SpriteBatchRegistry.cpp
    src/Radium/imgui_impl_rune.cpp
    src/Radium/imgui_impl_nova.cpp
    src/Radium/Input.cpp
    src/Radium/AssetLoader.cpp
    src/Radium/DebugRenderer.cpp
    src/Radium/Camera.cpp
    src/Radium/SubViewport.cpp
    src/Radium/Nodes/Tree.cpp
    src/Radium/Nodes/Node.cpp
    src/Radium/Nodes/LuaScript.cpp
    src/Radium/Nodes/ClassDB.cpp
    src/Radium/Nodes/2D/Node2D.cpp
    src/Radium/Nodes/2D/Sprite2D.cpp
    src/Radium/Nodes/2D/TileMap2D.cpp
    src/Radium/Nodes/2D/RigidBody.cpp
    subprojects/lua/onelua.c
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Radium PRIVATE TRACY_ENABLE)
else()
    # Disable profiling in Release
    target_compile_definitions(Radium PRIVATE TRACY_NO_INVARIANT_CHECK=1)
endif()


target_include_directories(Radium PUBLIC subprojects/lua)

target_link_libraries(Radium PUBLIC
    Nova
    Iris
    Flux
    Rune
    imgui
    box2d
    tiny-process-library
)
if (MSVC)
    target_link_libraries(Radium PUBLIC dbghelp)
endif()
target_include_directories(Radium PUBLIC
    src
    subprojects/ImGuiColorTextEdit/
    subprojects/ImGuiFileDialog/
)

target_link_libraries(Radium PRIVATE TracyClient)
target_compile_definitions(Radium PRIVATE TRACY_ENABLE)

# MSVC: fix C1128 (section count) on large translation units like ChaiScript.cpp
if (MSVC)
    target_compile_options(Radium PRIVATE /bigobj)
    target_compile_options(imgui PRIVATE /bigobj)
endif()

if (EMSCRIPTEN)
    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sALLOW_MEMORY_GROWTH -s USE_WEBGPU=1 -sASYNCIFY -s WASM=1 -g4 -s ASSERTIONS=1")

    # Generate .html output
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

if (NOT ANDROID AND NOT IOS)
    add_executable(RadiumRuntime
        src/RadiumRuntime/main.cpp
    )
else()
    if (ANDROID)
        add_library(RadiumRuntime SHARED
            src/RadiumRuntime/main.cpp
        )
    else()

        # Path to your assets folder
        set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/src/RadiumRuntime/assets")

        # Get all files recursively
        file(GLOB_RECURSE ASSET_FILES
            "${ASSETS_DIR}/*"
        )

        foreach(asset ${ASSET_FILES})
            set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/RadiumRuntime")

            file(RELATIVE_PATH REL_PATH "${BASE_DIR}" "${asset}")

            message(STATUS "Asset path: ${asset}")
            message(STATUS "Relative path: ${REL_PATH}")

            # Set the MACOSX_PACKAGE_LOCATION to Resources/assets/[subdirs]
            set_source_files_properties(${asset} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/assets/${REL_PATH}"
            )
        endforeach()

        # Add executable
        add_executable(RadiumRuntime MACOSX_BUNDLE
            src/RadiumRuntime/main.cpp
            ${ASSET_FILES}
        )

        

        set_target_properties(RadiumRuntime PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.radium.TestApp"
            MACOSX_BUNDLE_BUNDLE_NAME "TestApp"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
            MACOSX_BUNDLE_INFO_STRING "My Radium App"
            RESOURCE "${ASSET_FILES}" # optional folder for assets
        )



    endif()
endif()

target_link_libraries(RadiumRuntime PUBLIC Radium)


if (EMSCRIPTEN)
    set_target_properties(RadiumRuntime PROPERTIES
        LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/examples/angry-birds@/"
    )
endif ()



if(APPLE AND IOS)
    target_compile_definitions(Radium PRIVATE TARGET_OS_IOS=1)
endif()


if (NOT ANDROID AND NOT IOS)
add_executable(Editor src/Editor/main.cpp)
target_link_libraries(Editor PUBLIC Radium)
endif()



include(FetchContent)

# 1. Fetch the doxygen-awesome-css theme
FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# 2. Find Doxygen
find_package(Doxygen)

if (DOXYGEN_FOUND)
    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")

    # Fetch the Doxygen Awesome CSS theme
    include(FetchContent)
    FetchContent_Declare(
        doxygen-awesome-css
        URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    # Set variables for Doxyfile.in substitution
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_HTML YES)

    # Configure Doxyfile
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_BINARY_DIR}/Doxyfile
        @ONLY
    )

    # Add a doc target
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen + Awesome CSS"
        VERBATIM
    )
else()
    message(WARNING "Doxygen not found. Documentation will not be generated.")
endif()

