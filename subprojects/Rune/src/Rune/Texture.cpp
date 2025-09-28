#include <Rune/Texture.hpp>

namespace Rune {
    Texture::Texture(int width, int height, void* data) {
        WGPUTextureDescriptor textureDesc = {};
        textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.size = {(uint32_t)width, (uint32_t)height, 1};
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;

        WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDesc);

        textureView = wgpuTextureCreateView(texture, nullptr);

        WGPUSamplerDescriptor samplerDesc = {};
        samplerDesc.magFilter = WGPUFilterMode_Nearest;
        samplerDesc.minFilter = WGPUFilterMode_Nearest;
        samplerDesc.maxAnisotropy = 1;
        sampler = wgpuDeviceCreateSampler(device, &samplerDesc);

#ifndef __EMSCRIPTEN__
        WGPUTexelCopyTextureInfo copyDst = {};
#else
        WGPUImageCopyTexture copyDst = {};

#endif
        copyDst.texture = texture;
        copyDst.mipLevel = 0;
        copyDst.origin = {0, 0, 0};
        copyDst.aspect = WGPUTextureAspect_All;

#ifndef __EMSCRIPTEN__
        WGPUTexelCopyBufferLayout dataLayout = {};
#else
        WGPUTextureDataLayout dataLayout = {};
#endif
        dataLayout.offset = 0;
        dataLayout.bytesPerRow = static_cast<uint32_t>(width * 4);
        dataLayout.rowsPerImage = static_cast<uint32_t>(height);

        WGPUExtent3D writeSize = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1};

        wgpuQueueWriteTexture(
            queue,
            &copyDst,
            data,
            width * width * 4,
            &dataLayout,
            &writeSize);
    }

    void Texture::Destroy() {
        wgpuTextureViewRelease(textureView);
        wgpuSamplerRelease(sampler);
    }
}