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

#define MAX_SPRITES 512

namespace Rune {
    struct SpriteData {
        float pos[2];
        float size[2];
        float color[3];
        float rotation;
        float uvOffset[2];
        float uvScale[2];
    };

    struct OtherDataUniform {
        float screenWidth;
        float screenHeight;
        int spriteOrigin;
        int pad;
    };

    enum class SpriteOrigin {
        TopLeft,
        Center
    };


    static_assert(sizeof(SpriteData) == 48, "SpriteData must be 48 bytes");


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
        SpriteBatch(int texWidth, int texHeight, void* data, SpriteOrigin);
        void Begin();

        void Draw(float x, float y, uint w, uint h, float r, float g, float b, float rotation = 0);

        void DrawTile(
            float x, float y, uint w, uint h,
            float r, float g, float b,
            uint tileX, uint tileY,
            uint tilesPerRow, uint tilesPerCol,
            float rotation = 0
        );
        void DrawImageRect(float x, float y, uint w, uint h,
                   float r, float g, float b,
                   uint srcX, uint srcY, uint srcW, uint srcH,
                   uint textureWidth, uint textureHeight,
                   float rotation);


        void End();

        void Destroy();
    };
}