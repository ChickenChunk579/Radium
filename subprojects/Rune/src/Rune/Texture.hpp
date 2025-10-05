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

    enum class TextureFormat {
        RGBA8,
        BGRA8,
        RGB8
    };

    class Texture {
    public:
        Texture(int width, int height, int pitch, void* data, SamplingMode samplingmode, TextureFormat format = TextureFormat::RGBA8);

        Texture(WGPUTextureView textureView, SamplingMode samplingMode);

        ~Texture();

        WGPUTextureView textureView;
        WGPUTexture texture;
        WGPUSampler sampler;

        void Destroy();
    };
}