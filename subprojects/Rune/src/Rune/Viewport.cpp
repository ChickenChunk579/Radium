#include <Rune/Viewport.hpp>

namespace Rune {
    Viewport::Viewport(int width, int height) {

        this->width = width;
        this->height = height;

        WGPUTextureDescriptor textureDesc = {};
        textureDesc.size.width = width;
        textureDesc.size.height = height;
        textureDesc.size.depthOrArrayLayers = 1;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
        textureDesc.usage = WGPUTextureUsage_CopySrc | WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
        textureDesc.label = {"Rune Viewport", 13};
        textureDesc.viewFormatCount = 0;
        textureDesc.viewFormats = nullptr;

        WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDesc);

        WGPUTextureViewDescriptor viewDesc = {};
        viewDesc.nextInChain = nullptr;
        viewDesc.format = WGPUTextureFormat_RGBA8Unorm;
        viewDesc.dimension = WGPUTextureViewDimension_2D;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = 1;
        viewDesc.aspect = WGPUTextureAspect_All;
        textureView = wgpuTextureCreateView(texture, &viewDesc);
    }

    void Viewport::SetupFrame() {
        WGPURenderPassDescriptor passDesc = {};
        passDesc.label = {"", 0};
        passDesc.colorAttachmentCount = 1;
        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.view = textureView;
        colorAttachment.resolveTarget = NULL;
        colorAttachment.loadOp = WGPULoadOp_Clear;
        colorAttachment.clearValue = {};
        colorAttachment.clearValue.r = 1;
        colorAttachment.clearValue.g = 0;
        colorAttachment.clearValue.b = 0;
        colorAttachment.clearValue.a = 1;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.depthSlice = NULL;
        passDesc.colorAttachments = &colorAttachment;
        passDesc.depthStencilAttachment = NULL;
        passDesc.occlusionQuerySet = NULL;
        passDesc.timestampWrites = NULL;

        renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &passDesc);
    }

    void Viewport::FinishFrame() {
        WGPUCommandBufferDescriptor cmdBufDesc = {};
        cmdBufDesc.label = {"", 0};
        cmdBufDesc.nextInChain = nullptr;

        wgpuRenderPassEncoderEnd(renderPass);
    }


}