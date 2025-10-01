#pragma once
#include <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

WGPUSurface CreateSurfaceForWindow(void* nsWindow, WGPUInstance instance);

#ifdef __cplusplus
}
#endif
