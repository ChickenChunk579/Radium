#include <webgpu/webgpu.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

WGPUSurface CreateSurfaceForWindow(void* windowRaw, WGPUInstance instance) {
    if (!windowRaw || !instance) {
        return NULL;
    }

    NSWindow* window = (NSWindow*)windowRaw;

    // Ensure the window's content view has a layer
    [window.contentView setWantsLayer:YES];

    // Create a Metal layer
    CAMetalLayer *metalLayer = [CAMetalLayer layer];
    [window.contentView setLayer:metalLayer];

    // Prepare the WebGPU Metal layer chain
    WGPUSurfaceSourceMetalLayer fromMetalLayer;
    fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
    fromMetalLayer.chain.next = NULL;
    fromMetalLayer.layer = metalLayer;

    // Prepare the surface descriptor
    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
    surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

    // Create and return the WebGPU surface
    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
}


#ifdef __cplusplus
}
#endif