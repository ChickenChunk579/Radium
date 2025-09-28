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
    enum class SamplingMode {
        Nearest,
        Linear
    };

    class Texture {
    public:
        Texture(int width, int height, void* data, SamplingMode samplingmode);

        WGPUTextureView textureView;
        WGPUSampler sampler;

        void Destroy();
    };
}