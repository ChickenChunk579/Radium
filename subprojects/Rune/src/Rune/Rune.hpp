#pragma once

#include <cstdint>
#include <string>
#include <webgpu/webgpu.h> // W3C header
#ifndef __EMSCRIPTEN__
#include <webgpu/wgpu.h>   // Native header
#endif
#include <vector>

#if defined(__linux__)
    #include <X11/Xlib.h>
#else
    typedef void Display;
#endif


#ifdef __EMSCRIPTEN__
#define WGPUOptionalBool_True true
#define WGPUOptionalBool_False false
#endif


namespace Rune {

    extern WGPUInstance instance;
    extern WGPUSurface surface;
    extern WGPUAdapter adapter;
    extern WGPUDevice device;
    extern WGPURenderPassEncoder activeRenderPass;
    extern WGPURenderPassEncoder windowRenderPass;
    extern WGPUTextureView targetView;
    extern WGPUCommandEncoder encoder;
    extern WGPUQueue queue;
    extern WGPUSurfaceCapabilities caps;
    extern WGPUTextureView depthTextureView;
    extern bool ready;
    extern float windowWidth;
    extern float windowHeight;
    extern std::vector<WGPUCommandBuffer> commandBuffers;

    // Initializes the renderer and underlying wgpu instance
    bool Initialize(Display* x11Display, uint32_t x11Window, uint32_t width, uint32_t height);

    // Shuts down and cleans up
    void Shutdown();

    // Create encoder
    void PreSetupFrame();

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
