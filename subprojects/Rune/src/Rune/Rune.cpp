#include "Rune/Rune.hpp"
#include <iostream>
#include <webgpu/webgpu.h> // Make sure this is on your include path
#include <cassert>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu.h>
#include <emscripten/html5.h>
#else
#include <webgpu/wgpu.h> // Make sure this is on your include path
#endif
#include <Rune/TraceHooks.hpp>
#include <sstream>


#if defined(__APPLE__)
#include "Rune/objc_bridge.h"
#endif

WGPUAdapter requestAdapterSync(WGPUInstance instance, const WGPURequestAdapterOptions *options)
{
#ifndef __EMSCRIPTEN__
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void *userdata1, void *userdata2)
    {
        auto &userData = *reinterpret_cast<UserData *>(userdata1);
        if (status == WGPURequestAdapterStatus_Success)
        {
            userData.adapter = adapter;
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to get WebGPU adapter";
            if (message.length > 0 && message.data)
                ss << ": " << message.data;
            Rune::Log(ss.str());

        }
        userData.requestEnded = true;
    };

    WGPURequestAdapterCallbackInfo callbackInfo = {};
    callbackInfo.callback = onAdapterRequestEnded;
    callbackInfo.userdata1 = &userData;
    callbackInfo.userdata2 = nullptr;
    callbackInfo.nextInChain = nullptr;

    // Call to request the adapter
    wgpuInstanceRequestAdapter(instance, options, callbackInfo);

    // Spin-wait until the callback finishes (only valid for native builds)
    while (!userData.requestEnded)
    {
    }

    assert(userData.requestEnded);
    return userData.adapter;
#else
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    // Callback called by wgpuInstanceRequestAdapter when the request returns
    // This is a C++ lambda function, but could be any function defined in the
    // global scope. It must be non-capturing (the brackets [] are empty) so
    // that it behaves like a regular C function pointer, which is what
    // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
    // is to convey what we want to capture through the pUserData pointer,
    // provided as the last argument of wgpuInstanceRequestAdapter and received
    // by the callback as its last argument.
    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message, void *pUserData)
    {
        UserData &userData = *reinterpret_cast<UserData *>(pUserData);
        if (status == WGPURequestAdapterStatus_Success)
        {
            userData.adapter = adapter;
        }
        else
        {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    // Call to the WebGPU request adapter procedure
    wgpuInstanceRequestAdapter(
        instance /* equivalent of navigator.gpu */,
        options,
        onAdapterRequestEnded,
        (void *)&userData);

    // We wait until userData.requestEnded gets true
#ifdef __EMSCRIPTEN__
    while (!userData.requestEnded)
    {
        emscripten_sleep(100);
    }
#endif // __EMSCRIPTEN__

    assert(userData.requestEnded);

    return userData.adapter;
#endif
}

WGPUDevice requestDeviceSync(WGPUAdapter instance, WGPUDeviceDescriptor const *descriptor)
{
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData
    {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

// Callback called by wgpuInstanceRequestAdapter when the request returns
// This is a C++ lambda function, but could be any function defined in the
// global scope. It must be non-capturing (the brackets [] are empty) so
// that it behaves like a regular C function pointer, which is what
// wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
// is to convey what we want to capture through the pUserData pointer,
// provided as the last argument of wgpuInstanceRequestAdapter and received
// by the callback as its last argument.
#ifndef __EMSCRIPTEN__
    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void *userdata1, void *userdata2)
#else
    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, const char *message, void *userdata1)
#endif
    {
        UserData &userData = *reinterpret_cast<UserData *>(userdata1);
        if (status == WGPURequestDeviceStatus_Success)
        {
            userData.device = device;
        }
        else
        {
            Rune::Log("Could not get WebGPU device");
        }
        userData.requestEnded = true;
    };

#ifndef __EMSCRIPTEN__
    WGPURequestDeviceCallbackInfo callbackInfo = {};
    callbackInfo.callback = onDeviceRequestEnded;
    callbackInfo.userdata1 = (void *)&userData;
    callbackInfo.userdata2 = nullptr;
    callbackInfo.nextInChain = nullptr;

    // Call to the WebGPU request adapter procedure
    wgpuAdapterRequestDevice(
        instance /* equivalent of navigator.gpu */,
        descriptor,
        callbackInfo);
#else
    wgpuAdapterRequestDevice(
        instance /* equivalent of navigator.gpu */,
        descriptor,
        onDeviceRequestEnded,
        (void *)&userData);
#endif

    // We wait until userData.requestEnded gets true
#ifdef __EMSCRIPTEN__
    while (!userData.requestEnded)
    {
        emscripten_sleep(100);
    }
#endif // __EMSCRIPTEN__

    assert(userData.requestEnded);

    return userData.device;
}

#ifdef __EMSCRIPTEN__
void requestAdapterAsync(WGPUInstance instance, const WGPURequestAdapterOptions* options,
                         void (*onDone)(WGPUAdapter, void*), void* userdata) {
    struct AdapterCallbackData {
        void (*onDone)(WGPUAdapter, void*);
        void* userdata;
    };

    auto* data = new AdapterCallbackData{ onDone, userdata };

    auto callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* pUserData) {
        auto* data = static_cast<AdapterCallbackData*>(pUserData);
        if (status != WGPURequestAdapterStatus_Success) {
            printf("Failed to get adapter: %s\n", message);
            data->onDone(nullptr, data->userdata);
        } else {
            data->onDone(adapter, data->userdata);
        }
        delete data;
    };

    wgpuInstanceRequestAdapter(instance, options, callback, data);
}


void requestDeviceAsync(WGPUAdapter adapter, const WGPUDeviceDescriptor* desc,
                        void (*onDone)(WGPUDevice, void*), void* userdata) {
    struct DeviceCallbackData {
        void (*onDone)(WGPUDevice, void*);
        void* userdata;
    };

    auto* data = new DeviceCallbackData{ onDone, userdata };

    auto callback = [](WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* pUserData) {
        auto* data = static_cast<DeviceCallbackData*>(pUserData);
        if (status != WGPURequestDeviceStatus_Success) {
            printf("Failed to get device: %s\n", message);
            data->onDone(nullptr, data->userdata);
        } else {
            data->onDone(device, data->userdata);
        }
        delete data;
    };

    wgpuAdapterRequestDevice(adapter, desc, callback, data);
}

#endif

namespace Rune
{

    // WebGPU objects (barebones)
    WGPUInstance instance = nullptr;
    WGPUSurface surface = nullptr;
    WGPUAdapter adapter = nullptr;
    WGPUDevice device = nullptr;
    WGPURenderPassEncoder activeRenderPass = nullptr;
    WGPURenderPassEncoder windowRenderPass = nullptr;
    Viewport* currentViewport = nullptr;
    WGPUTextureView targetView = nullptr;
    WGPUCommandEncoder encoder = nullptr;
    WGPUSurfaceCapabilities caps;
    WGPUTextureView depthTextureView = nullptr;
    bool ready = false;
    float windowWidth, windowHeight = 0;
    std::vector<WGPUCommandBuffer> commandBuffers;

    float clearColorR = 0;
    float clearColorG = 0;
    float clearColorB = 0;
    float clearColorA = 0;
    WGPUQueue queue;

    WGPUTextureView GetNextSurfaceTextureView()
    {
        #ifndef DOTNET
        // Get the surface texture
        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
#ifndef __EMSCRIPTEN__
        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
#else
        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success)
#endif
        {
            return nullptr;
        }

        // Create a view for this surface texture
        WGPUTextureViewDescriptor viewDescriptor = {};
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.format = caps.formats[0];
        viewDescriptor.dimension = WGPUTextureViewDimension_2D;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.mipLevelCount = 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.arrayLayerCount = 1;
        viewDescriptor.aspect = WGPUTextureAspect_All;
        WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

        return targetView;
        #else
        return nullptr;
        #endif
    }

    bool Initialize(Display *x11Display, uint32_t x11Window, uint width, uint height)
    {
        windowWidth = width;
        windowHeight = height;

        setenv("RUST_BACKTRACE", "1", 1);
        
        std::cout << "[Rune] Initializing renderer..." << std::endl;

        // We create a descriptor
        #ifndef __EMSCRIPTEN__
        WGPUInstanceDescriptor desc = {};
        desc.nextInChain = nullptr;
        WGPUInstanceExtras extras = {};
        extras.chain.sType = (WGPUSType)WGPUSType_InstanceExtras;
        extras.chain.next = nullptr;
        extras.backends = WGPUBackendType_OpenGL; // Or Auto
        extras.dx12ShaderCompiler = WGPUDx12Compiler_Fxc;

        desc.nextInChain = &extras.chain;
        // We create the instance using this descriptor
        instance = wgpuCreateInstance(&desc);

        #else

        #ifndef DOTNET
        instance = wgpuCreateInstance(nullptr);
        #endif

        #endif


        #ifndef DOTNET
        if (!instance) {
            Rune::Log("Failed to create WGPUInstance!");
            return false;
        } else {
            Rune::Log("Created WGPUInstance successfully.");
        }

        #else
        std::cout << "[Rune] Instance skipped - running in dotnet WASM";
        #endif


        Log("Created instance");

        

        #ifndef __EMSCRIPTEN__
        #ifndef __ANDROID__
        #ifdef __linux__

        WGPUSurfaceSourceXlibWindow fromXlibWindow;
        fromXlibWindow.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
        fromXlibWindow.chain.next = NULL;
        fromXlibWindow.display = x11Display;
        fromXlibWindow.window = x11Window;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromXlibWindow.chain;
        surfaceDescriptor.label = (WGPUStringView){NULL, WGPU_STRLEN};

        surface = wgpuInstanceCreateSurface(instance, &surfaceDescriptor);

        #else
        surface = CreateSurfaceForWindow(x11Display, instance);
        
        #endif

        #else

        WGPUSurfaceSourceAndroidNativeWindow fromAndroid;
        fromAndroid.chain.sType = WGPUSType_SurfaceSourceAndroidNativeWindow;
        fromAndroid.chain.next = NULL;
        fromAndroid.window = (void*)x11Display;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromAndroid.chain;
        surfaceDescriptor.label = (WGPUStringView){NULL, WGPU_STRLEN};

        surface = wgpuInstanceCreateSurface(instance, &surfaceDescriptor);

        #endif

        #else

        WGPUSurfaceDescriptorFromCanvasHTMLSelector fromCanvasHTMLSelector;
        fromCanvasHTMLSelector.chain.next = NULL;
        fromCanvasHTMLSelector.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
        fromCanvasHTMLSelector.selector = "canvas";

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector.chain;
        surfaceDescriptor.label = NULL;

        surface = wgpuInstanceCreateSurface(instance, &surfaceDescriptor);

        #endif

        Log("Created surface");

        WGPURequestAdapterOptions adapterOpts = {};
        adapterOpts.compatibleSurface = surface;
        adapterOpts.backendType = WGPUBackendType_OpenGL;
        adapterOpts.nextInChain = nullptr;

        adapter = requestAdapterSync(instance, &adapterOpts);
        Log("Got adapter");

        WGPUDeviceDescriptor deviceDesc = {};
        deviceDesc.nextInChain = nullptr;
        #ifndef __EMSCRIPTEN__
        deviceDesc.requiredFeatureCount = 1;
        WGPUNativeFeature features[] = {
            WGPUNativeFeature_VertexWritableStorage};
        deviceDesc.requiredFeatures = (WGPUFeatureName *)features;
        #else
        deviceDesc.requiredFeatureCount = 0;
        deviceDesc.requiredFeatures = nullptr;
        #endif

        // Get adapter limits
        #ifndef __EMSCRIPTEN__
        WGPULimits supportedLimits = {};
        #else
        WGPUSupportedLimits supportedLimits = {};
        #endif
        wgpuAdapterGetLimits(adapter, &supportedLimits);

        // Set required limits
        #ifndef __EMSCRIPTEN__
        WGPULimits requiredLimits = supportedLimits;
        deviceDesc.requiredLimits = &requiredLimits;
        #else
        deviceDesc.requiredLimits = nullptr;
        #endif

        // Default queue
        WGPUQueueDescriptor queueDesc = {};
        queueDesc.nextInChain = nullptr;
        deviceDesc.defaultQueue = queueDesc;

        // Device lost callback
        #ifndef __EMSCRIPTEN__
        auto deviceLostCallback = [](const WGPUDevice *, WGPUDeviceLostReason reason, WGPUStringView message, void *, void *)
        {
            std::stringstream ss;
            ss << "Device lost! Reason: " << reason;
            if (message.length > 0)
                ss << " (" << message.data << ")";
            Rune::Log(ss.str());

        };
        #else
        auto deviceLostCallback = [](WGPUDeviceLostReason reason, const char* message, void *)
        {
            std::cout << "Device lost! Reason: " << reason;
            std::cout << " (" << message << ")";
            std::cout << std::endl;
        };
        #endif

        #ifndef __EMSCRIPTEN__
        WGPUDeviceLostCallbackInfo lostCallbackInfo = {};
        lostCallbackInfo.callback = deviceLostCallback;
        lostCallbackInfo.userdata1 = nullptr;
        lostCallbackInfo.userdata2 = nullptr;
        deviceDesc.deviceLostCallbackInfo = lostCallbackInfo;
        #else
        deviceDesc.deviceLostCallback = deviceLostCallback;
        #endif

        // Request device
        device = requestDeviceSync(adapter, &deviceDesc);
        if (!device)
        {
            Rune::Log("Failed to get WebGPU device!");

            return false;
        }

        queue = wgpuDeviceGetQueue(device);
        Log("Got device and queue");

        // Configure the surface
        WGPUSurfaceConfiguration config = {};
        config.nextInChain = nullptr;

        wgpuSurfaceGetCapabilities(surface, adapter, &caps);

        if (caps.formats[0] == WGPUTextureFormat_BGRA8UnormSrgb) {
            WGPUTextureFormat* mutableFormats = const_cast<WGPUTextureFormat*>(caps.formats);
            mutableFormats[0] = WGPUTextureFormat_BGRA8Unorm;

        }

        if (caps.formats[0] == WGPUTextureFormat_RGBA8UnormSrgb) {
            WGPUTextureFormat* mutableFormats = const_cast<WGPUTextureFormat*>(caps.formats);
            mutableFormats[0] = WGPUTextureFormat_RGBA8Unorm;

        }

        
        #if defined(__APPLE__)
        WGPUTextureFormat* mutableFormats = const_cast<WGPUTextureFormat*>(caps.formats);
        mutableFormats[0] = WGPUTextureFormat_BGRA8Unorm;
        #endif
        

        // Configuration of the textures created for the underlying swap chain
        config.width = width;
        config.height = height;
        config.usage = WGPUTextureUsage_RenderAttachment;
        WGPUTextureFormat surfaceFormat = caps.formats[0];
        config.format = surfaceFormat;

        // And we do not need any particular view format:
        config.viewFormatCount = 0;
        config.viewFormats = nullptr;
        config.device = device;
        config.presentMode = WGPUPresentMode_Fifo;
        config.alphaMode = WGPUCompositeAlphaMode_Auto;

        wgpuSurfaceConfigure(surface, &config);


        WGPUTextureFormat depthFormat = WGPUTextureFormat_Depth24Plus;
        WGPUTextureDescriptor depthTextureDesc = {};
        depthTextureDesc.dimension = WGPUTextureDimension_2D;
        depthTextureDesc.format = depthFormat;
        depthTextureDesc.mipLevelCount = 1;
        depthTextureDesc.sampleCount = 1;
        WGPUExtent3D size = {};
        size.width = static_cast<uint32_t>(windowWidth);
        size.height = static_cast<uint32_t>(windowHeight);
        size.depthOrArrayLayers = 1;

        std::cout << "Depth texture size: " << windowWidth << "x" << windowHeight << std::endl;

        size.depthOrArrayLayers = 1;
        depthTextureDesc.size = size;
        depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_RenderAttachment;
        depthTextureDesc.viewFormatCount = 0;
        depthTextureDesc.viewFormats = NULL;
        WGPUTexture depthTexture = wgpuDeviceCreateTexture(device, &depthTextureDesc);


        std::cout << "Made texture" << std::endl;

        WGPUTextureViewDescriptor depthTextureViewDesc = {};
        depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
        depthTextureViewDesc.baseArrayLayer = 0;
        depthTextureViewDesc.arrayLayerCount = 1;
        depthTextureViewDesc.baseMipLevel = 0;
        depthTextureViewDesc.mipLevelCount = 1;
        depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
        depthTextureViewDesc.format = depthFormat;
        depthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDesc);

        Rune::Log("Renderer initialized successfully");


        return true;
        
    }

    void Clear(float r, float g, float b, float a)
    {
        clearColorR = r;
        clearColorG = g;
        clearColorB = b;
        clearColorA = a;
    }


    void PreSetupFrame() {
        Rune::TraceZoneBegin("Get Target View");
        // Get the next target texture view
        targetView = GetNextSurfaceTextureView();
        if (!targetView)
            return;
        Rune::TraceZoneEnd();


        Rune::TraceZoneBegin("Create encoder");
        // Create a command encoder for the draw call
        WGPUCommandEncoderDescriptor encoderDesc = {};
        encoderDesc.nextInChain = nullptr;
        encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);
        Rune::TraceZoneEnd();
    }

    void SetupFrame()
    {
        


        Rune::TraceZoneBegin("Create render pass");
        // Create the render pass that clears the screen with our color
        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.nextInChain = nullptr;

        // The attachment part of the render pass descriptor describes the target texture of the pass
        WGPURenderPassColorAttachment renderPassColorAttachment = {};
        renderPassColorAttachment.view = targetView;
        renderPassColorAttachment.resolveTarget = nullptr;
        renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
        renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
        renderPassColorAttachment.clearValue = WGPUColor{clearColorR, clearColorG, clearColorB, clearColorA};
#ifndef WEBGPU_BACKEND_WGPU
        renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif // NOT WEBGPU_BACKEND_WGPU

        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &renderPassColorAttachment;
        renderPassDesc.depthStencilAttachment = nullptr;
        renderPassDesc.timestampWrites = nullptr;

        WGPURenderPassDepthStencilAttachment depthStencilAttachment;

        // The view of the depth texture
        depthStencilAttachment.view = depthTextureView;

        // The initial value of the depth buffer, meaning "far"
        depthStencilAttachment.depthClearValue = 1.0f;
        // Operation settings comparable to the color attachment
        depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        // we could turn off writing to the depth buffer globally here
        depthStencilAttachment.depthReadOnly = false;

        // Stencil setup, mandatory but unused
        depthStencilAttachment.stencilClearValue = 0;
        depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.stencilReadOnly = true;

        renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

        // Create the render pass and end it immediately (we only clear the screen but do not draw anything)
        windowRenderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
        Rune::TraceZoneEnd();

        activeRenderPass = windowRenderPass;
    }

    void ProcessEvents()
    {
    }

#ifdef __EMSCRIPTEN__

    struct raf_UserData
    {
        bool finished = false;
    };

    EM_BOOL FrameCallback(double time, void *userData)
    {
        raf_UserData *rafUserData = reinterpret_cast<raf_UserData *>(userData);
        rafUserData->finished = true;
        std::cout << "requestAnimationFrame at time: " << time << std::endl;
        return EM_FALSE; // Keep looping if needed
    }

#endif

    void FinishFrame()
    {
        wgpuRenderPassEncoderEnd(activeRenderPass);
        wgpuRenderPassEncoderRelease(activeRenderPass);

        // Finally encode and submit the render pass
        WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
        cmdBufferDescriptor.nextInChain = nullptr;
        WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
        wgpuCommandEncoderRelease(encoder);

        commandBuffers.push_back(command);

        wgpuQueueSubmit(queue, commandBuffers.size(), commandBuffers.data());
        wgpuCommandBufferRelease(command);

        commandBuffers.clear();

        // At the end of the frame
        wgpuTextureViewRelease(targetView);

        // wgpuDevicePoll(device, false, nullptr);

#ifndef __EMSCRIPTEN__
        wgpuSurfacePresent(surface);
#endif
    }

    void Shutdown()
    {
        std::cout << "[Rune] Shutting down renderer..." << std::endl;

        if (device)
        {
            wgpuDeviceRelease(device);
        }

        if (adapter)
        {
            wgpuAdapterRelease(adapter);
        }

        if (surface)
        {
            wgpuSurfaceRelease(surface);
        }

        if (instance)
        {
            wgpuInstanceRelease(instance);
        }

        std::cout << "[Rune] Shutdown complete." << std::endl;
    }

#ifdef __ANDROID__
#include <android/log.h>
#endif


    void Log(const std::string &message)
    {
        #ifndef __ANDROID__
        std::cout << "[Rune] " << message << std::endl;
        #else
        __android_log_print(ANDROID_LOG_INFO, "MyNativeApp", "%s", message.c_str());
        #endif
    }

}
