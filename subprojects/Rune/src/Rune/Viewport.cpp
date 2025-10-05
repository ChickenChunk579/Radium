#include <Rune/Viewport.hpp>
#include <Rune/Utils.hpp>

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
        textureDesc.format = caps.formats[0];
        textureDesc.usage = WGPUTextureUsage_CopySrc | WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
        textureDesc.label = CreateString("Rune Viewport");
        textureDesc.viewFormatCount = 0;
        textureDesc.viewFormats = nullptr;

        WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDesc);

        WGPUTextureViewDescriptor viewDesc = {};
        viewDesc.nextInChain = nullptr;
        viewDesc.format = caps.formats[0];
        viewDesc.dimension = WGPUTextureViewDimension_2D;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = 1;
        viewDesc.aspect = WGPUTextureAspect_All;
        textureView = wgpuTextureCreateView(texture, &viewDesc);
    
        WGPUTextureDescriptor depthDesc = {};
        depthDesc.dimension = WGPUTextureDimension_2D;
        depthDesc.format = WGPUTextureFormat_Depth24Plus;
        depthDesc.mipLevelCount = 1;
        depthDesc.sampleCount = 1;
        depthDesc.size.width = width;
        depthDesc.size.height = height;
        depthDesc.size.depthOrArrayLayers = 1;
        depthDesc.usage = WGPUTextureUsage_RenderAttachment;

        WGPUTexture depthTexture = wgpuDeviceCreateTexture(device, &depthDesc);

        WGPUTextureViewDescriptor depthViewDesc = {};
        depthViewDesc.aspect = WGPUTextureAspect_DepthOnly;
        depthViewDesc.baseArrayLayer = 0;
        depthViewDesc.arrayLayerCount = 1;
        depthViewDesc.baseMipLevel = 0;
        depthViewDesc.mipLevelCount = 1;
        depthViewDesc.dimension = WGPUTextureViewDimension_2D;
        depthViewDesc.format = WGPUTextureFormat_Depth24Plus;

        depthTextureView = wgpuTextureCreateView(depthTexture, &depthViewDesc);

    }

    void Viewport::SetupFrame() {
        WGPURenderPassDescriptor passDesc = {};
        passDesc.label = CreateString("");
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
        WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        depthStencilAttachment.view = depthTextureView;
        depthStencilAttachment.depthClearValue = 1.0f;
        depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.depthReadOnly = false;
        depthStencilAttachment.stencilClearValue = 0;
        depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.stencilReadOnly = true;

        passDesc.depthStencilAttachment = &depthStencilAttachment;

        passDesc.occlusionQuerySet = NULL;
        passDesc.timestampWrites = NULL;
        

        renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &passDesc);
        Activate();
    }

    void Viewport::FinishFrame() {
        WGPUCommandBufferDescriptor cmdBufDesc = {};
        cmdBufDesc.label = CreateString("");
        cmdBufDesc.nextInChain = nullptr;

        wgpuRenderPassEncoderEnd(renderPass);
        currentViewport = nullptr;
        activeRenderPass = windowRenderPass;
    }


    void Viewport::Activate() {
        activeRenderPass = renderPass;
        currentViewport = this;
    }
}