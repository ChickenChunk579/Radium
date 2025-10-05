#pragma once

#include <webgpu/webgpu.h>
#include <Rune/Rune.hpp>

namespace Rune {
    class Viewport {
    public:
        Viewport(int width, int height);
        void SetupFrame();
        void FinishFrame();

        void Activate();

        WGPUTextureView textureView;
        WGPUTextureView depthTextureView;

        int width, height;

    private:
        WGPURenderPassEncoder renderPass;

    };
}