#include <Rune/Texture.hpp>

namespace Rune
{
    WGPUFilterMode SamplingModeToWgpu(SamplingMode sampling)
    {
        switch (sampling)
        {
        case SamplingMode::Linear:
            return WGPUFilterMode_Linear;
        case SamplingMode::Nearest:
            return WGPUFilterMode_Nearest;
        default:
            return WGPUFilterMode_Nearest;
        }
    }

    Texture::Texture(int width, int height, int pitch, void *data, SamplingMode mode, TextureFormat format)
    {
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
        samplerDesc.magFilter = SamplingModeToWgpu(mode);
        samplerDesc.minFilter = SamplingModeToWgpu(mode);
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
        dataLayout.bytesPerRow = static_cast<uint32_t>(pitch); // use pitch here
        dataLayout.rowsPerImage = static_cast<uint32_t>(height);

        WGPUExtent3D writeSize = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1};

        wgpuQueueWriteTexture(
            queue,
            &copyDst,
            data,
            pitch * height, // total size of the buffer in bytes
            &dataLayout,
            &writeSize);
    }

    void Texture::Destroy()
    {
        wgpuTextureViewRelease(textureView);
        wgpuSamplerRelease(sampler);
    }
}