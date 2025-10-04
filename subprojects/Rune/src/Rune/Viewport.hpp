#pragma once

#include <webgpu/webgpu.h>
#include <Rune/Rune.hpp>

namespace Rune {
    class Viewport {
    public:
        Viewport(int width, int height);
        void SetupFrame();
        void FinishFrame();

        WGPUTextureView textureView;

        int width, height;

    private:
        WGPURenderPassEncoder renderPass;

    };
}