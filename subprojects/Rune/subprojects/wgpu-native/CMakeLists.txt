cmake_minimum_required(VERSION 3.16)
project(wgpu_native_wrapper LANGUAGES C CXX)

include(FetchContent)

# Detect target platform and architecture
if(ANDROID)
    set(WGPU_PLATFORM "android-x86_64")
    set(WGPU_ZIP "wgpu-android-x86_64-debug.zip")
elseif(UNIX AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(WGPU_PLATFORM "linux-x86_64")
    set(WGPU_ZIP "wgpu-linux-x86_64-debug.zip")
elseif(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    # Distinguish iOS vs macOS on Apple Silicon
    if(IOS)
        set(WGPU_PLATFORM "ios-aarch64-simulator")
        set(WGPU_ZIP "wgpu-ios-aarch64-simulator-debug.zip")
    else()
        set(WGPU_PLATFORM "macos-aarch64")
        set(WGPU_ZIP "wgpu-macos-aarch64-debug.zip")
    endif()
elseif(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(WGPU_PLATFORM "macos-x86_64")
    set(WGPU_ZIP "wgpu-macos-x86_64-debug.zip")
else()
    message(FATAL_ERROR "Unsupported platform or architecture")
endif()

# Base download URL and output path
set(WGPU_VERSION "v25.0.2.2")
set(WGPU_URL "https://github.com/gfx-rs/wgpu-native/releases/download/${WGPU_VERSION}/${WGPU_ZIP}")
set(WGPU_ZIP_PATH "${CMAKE_BINARY_DIR}/${WGPU_ZIP}")
set(WGPU_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${WGPU_PLATFORM}")

# Download the archive if it doesn't exist
if(NOT EXISTS "${WGPU_ZIP_PATH}")
    message(STATUS "Downloading wgpu-native from ${WGPU_URL}")
    file(DOWNLOAD
        "${WGPU_URL}"
        "${WGPU_ZIP_PATH}"
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS
    )
    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT)
    if(NOT DOWNLOAD_RESULT EQUAL 0)
        message(FATAL_ERROR "Download failed: ${DOWNLOAD_STATUS}")
    endif()
endif()

# Extract the archive if needed
if(NOT EXISTS "${WGPU_EXTRACT_DIR}/lib/libwgpu_native.a")
    message(STATUS "Extracting ${WGPU_ZIP_PATH} to ${WGPU_EXTRACT_DIR}")
    file(MAKE_DIRECTORY "${WGPU_EXTRACT_DIR}")
    file(ARCHIVE_EXTRACT
        INPUT "${WGPU_ZIP_PATH}"
        DESTINATION "${WGPU_EXTRACT_DIR}"
    )
endif()

# Header path
set(WGPU_INCLUDE_DIR "${WGPU_EXTRACT_DIR}/include")

# Static library path
set(WGPU_STATIC_LIB "${WGPU_EXTRACT_DIR}/lib/libwgpu_native.a")

# Check that required files exist
if(NOT EXISTS "${WGPU_STATIC_LIB}")
    message(FATAL_ERROR "Missing static library: ${WGPU_STATIC_LIB}")
endif()

if(NOT EXISTS "${WGPU_INCLUDE_DIR}/webgpu/wgpu.h")
    message(FATAL_ERROR "Missing header files in: ${WGPU_INCLUDE_DIR}")
endif()

# Define the imported target
add_library(wgpu::native STATIC IMPORTED GLOBAL)

set_target_properties(wgpu::native PROPERTIES
    IMPORTED_LOCATION "${WGPU_STATIC_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${WGPU_INCLUDE_DIR}"
)

message(STATUS "Configured wgpu::native for platform: ${WGPU_PLATFORM}")
message(STATUS "  Include dir: ${WGPU_INCLUDE_DIR}")
message(STATUS "  Library:     ${WGPU_STATIC_LIB}")
