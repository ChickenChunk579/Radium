#pragma once

#include <cstdint>
#include <string>
#include <webgpu/webgpu.h> // Make sure this is on your include path
#ifndef __EMSCRIPTEN__
#include <webgpu/wgpu.h> // Make sure this is on your include path
#endif
#ifdef __ANDROID__
typedef void Display;
#else
#include <X11/Xlib.h>
#endif

namespace Rune {

    extern WGPUInstance instance;
    extern WGPUSurface surface;
    extern WGPUAdapter adapter;
    extern WGPUDevice device;
    extern WGPURenderPassEncoder renderPass;
    extern WGPUTextureView targetView;
    extern WGPUCommandEncoder encoder;
    extern WGPUQueue queue;
    extern WGPUSurfaceCapabilities caps;
    extern bool ready;
    extern float windowWidth;
    extern float windowHeight;

    // Initializes the renderer and underlying wgpu instance
    bool Initialize(Display* x11Display, uint32_t x11Window, uint width, uint height);

    // Shuts down and cleans up
    void Shutdown();

    // Sets up frame to draw
    void SetupFrame();

    // Finishes frame
    void FinishFrame();

    // Process ImGui events
    void ProcessEvents();

    void Clear(float r, float g, float b, float a);

    // For logging (optional)
    void Log(const std::string& message);

}
