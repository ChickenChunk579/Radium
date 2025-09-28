#pragma once
#include "Rune.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu.h>
#else
#include <webgpu/wgpu.h>   // Make sure this is on your include path
#endif
#include <vector>
#include <iostream>
#include <assert.h>
#include <cstring>

namespace Rune {
    #ifdef __EMSCRIPTEN__
    inline const char* CreateString(const char* str) {
        return str;
    }
    #else
    inline WGPUStringView CreateString(const char* str) {
        return {str, strlen(str)};
    }
    #endif
}