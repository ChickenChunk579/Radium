#pragma once
#include "Rune.hpp"
#include <Rune/Texture.hpp>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu.h>
#else
#include <webgpu/wgpu.h>   // Make sure this is on your include path
#endif
#include <Rune/Utils.hpp>
#include <vector>
#include <iostream>
#include <assert.h>

namespace Rune
{

    class GeometryRenderer
    {
    public:
        GeometryRenderer(Rune::Texture* texture);
        ~GeometryRenderer();

        void SetVertices(const std::vector<float> &vertices); // [x, y, r, g, b] * n
        void Draw();
        void SetTexture(Rune::Texture* newTexture);
    private:
        Rune::Texture* texture;
    bool ownedFallbackTexture = false;

        WGPUBuffer vertexBuffer = nullptr;
        WGPUBuffer screenSizeBuffer = nullptr;
    size_t vertexBufferCapacity = 0;
    std::vector<WGPUBuffer> oldVertexBuffers; // buffers waiting for safe destruction

        WGPUBindGroup bindGroup = nullptr;
        WGPUBindGroupLayout bindGroupLayout = nullptr;
        WGPURenderPipeline pipeline = nullptr;

        size_t vertexCount = 0;

        void CreatePipeline();
    };
}