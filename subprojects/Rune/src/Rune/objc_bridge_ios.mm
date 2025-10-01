#include <webgpu/webgpu.h>
#  include <UIKit/UIKit.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#  include <Metal/Metal.h>
#import <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

WGPUSurface CreateSurfaceForWindow(void* windowRaw, WGPUInstance instance) {
    if (!windowRaw || !instance) {
        return NULL;
    }

    UIWindow* ui_window = (__bridge UIWindow*)windowRaw;
    UIView* ui_view = ui_window.rootViewController.view;
    CAMetalLayer* metal_layer = [CAMetalLayer new];
    metal_layer.opaque = true;
    metal_layer.frame = ui_view.frame;
    metal_layer.drawableSize = ui_view.frame.size;

    [ui_view.layer addSublayer: metal_layer];

    WGPUSurfaceSourceMetalLayer fromMetalLayer;
    fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
    fromMetalLayer.chain.next = NULL;
    fromMetalLayer.layer = (__bridge void*)metal_layer;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
    surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);

}


#ifdef __cplusplus
}
#endif
