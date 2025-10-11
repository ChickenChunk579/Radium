#pragma once
#include "Rune.hpp"
#include "Rune/Texture.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu.h>
#else
#include <webgpu/wgpu.h>   // Make sure this is on your include path
#endif
#include <vector>
#include <iostream>
#include <assert.h>

#define MAX_SPRITES 1024

namespace Rune {
    struct SpriteData {
        float color[3];   // 12 bytes
        uint32_t flags;   // 4 bytes, now at offset 12

        float pos[2];     // offset 16
        float z;          // offset 24
        float pad1;       // offset 28

        float size[2];    // offset 32
        float rotation;   // offset 40
        float pad2;       // offset 44

        float uvOffset[2]; // offset 48
        float uvScale[2];  // offset 56
    };
    static_assert(sizeof(SpriteData) == 64, "Wrong SpriteData size");



    struct OtherDataUniform {
        float screenWidth;
        float screenHeight;
        int spriteOrigin;
        int pad;
    };

    enum class SpriteOrigin {
        TopLeft = 0,
        Center = 1
    };


    class SpriteBatch {
    public:
        SpriteData* uniformWorking;
        WGPURenderPipeline pipeline;
        WGPUBindGroupLayout bindGroupLayout;
        WGPUBindGroup bindGroup;
        WGPUTextureView textureView;
        bool bindGroupCreated = false;
        WGPUBuffer vertexBuffer;
        WGPUSampler sampler;
        WGPUBuffer resolutionBuffer;
        size_t objectCount = 0;

        WGPUBuffer uniformBuffer;
        size_t uniformBufferSize;
        bool bufferCreated = false;

        size_t throttledCount = 0;

        int textureWidth = 0, textureHeight = 0;

        bool started = false;

        SpriteOrigin origin;
    public:
        SpriteBatch(Rune::Texture* texture, SpriteOrigin);
        void Begin();

        void Draw(float x, float y, uint32_t w, uint32_t h, float r, float g, float b, float rotation = 0, float z = 0, uint32_t flags = 0);

        void DrawTile(
            float x, float y, uint32_t w, uint32_t h,
            float r, float g, float b,
            uint32_t tileX, uint32_t tileY,
            uint32_t tilesPerRow, uint32_t tilesPerCol,
            float rotation = 0, float z = 0, uint32_t flags = 0
        );
        void DrawImageRect(float x, float y, uint32_t w, uint32_t h,
                   float r, float g, float b,
                   uint32_t srcX, uint32_t srcY, uint32_t srcW, uint32_t srcH,
                   uint32_t textureWidth, uint32_t textureHeight,
                   float rotation, float z = 0, uint32_t flags = 0);


        void End();

        void Destroy();
    };
}