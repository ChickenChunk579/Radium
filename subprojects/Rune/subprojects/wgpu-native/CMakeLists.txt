cmake_minimum_required(VERSION 3.16)
project(wgpu_native_wrapper LANGUAGES C CXX)

# Detect target platform and architecture
if(ANDROID)
    set(WGPU_PLATFORM "android-x86_64")
elseif(UNIX AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(WGPU_PLATFORM "linux-x86_64")
else()
    message(FATAL_ERROR "Unsupported platform or architecture")
endif()

# Base path to wgpu-native binaries
set(WGPU_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/${WGPU_PLATFORM}")

# Header path
set(WGPU_INCLUDE_DIR "${WGPU_ROOT}/include")

# Static library path
set(WGPU_STATIC_LIB "${WGPU_ROOT}/lib/libwgpu_native.a")

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
