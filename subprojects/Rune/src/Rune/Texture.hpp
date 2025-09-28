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

namespace Rune {
    class Texture {
    public:
        Texture(int width, int height, void* data);

        WGPUTextureView textureView;
        WGPUSampler sampler;

        void Destroy();
    };
}