#include <Rune/SpriteBatch.hpp>
#include <cstring>
#include <vector>
#include <Rune/Texture.hpp>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char *shaderSource = R"(

const MAX_SPRITES = 512;
struct SpriteData {
    color: vec3<f32>,       // offset 0 (12 bytes)
    flags: u32,             // offset 12 (matches uint8_t + padding on CPU)

    pos: vec2<f32>,         // offset 16
    z: f32,                 // offset 24
    pad1: f32,              // offset 28

    size: vec2<f32>,        // offset 32
    rotation: f32,          // offset 40
    pad2: f32,              // offset 44

    uvOffset: vec2<f32>,    // offset 48
    uvScale: vec2<f32>,     // offset 56
};




struct SpriteUniforms {
    sprites: array<SpriteData, MAX_SPRITES>
};

@group(0) @binding(0)
var<uniform> sprites: SpriteUniforms;

struct OtherDataUniform {
    screenSize: vec2f,
    origin: i32
};

@group(0) @binding(3)
var<uniform> otherData: OtherDataUniform;


struct VertexInput {
    @location(0) position: vec2f,
    @location(1) uv: vec2f,
    @builtin(instance_index) instanceIdx: u32
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
    @location(1) uv: vec2f
};

@vertex fn vs_main(input: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let screenSize = otherData.screenSize;
    var localPos = input.position;

    let sprite = sprites.sprites[input.instanceIdx];

    // Rotate the input.position
    let angle = (sprite.rotation / 180.0f) * 3.14159f;
    let cosA = cos(angle);
    let sinA = sin(angle);

    let rotated = vec2f(
        localPos.x * cosA - localPos.y * sinA,
        localPos.x * sinA + localPos.y * cosA
    );

    let worldPos = sprite.pos + rotated * sprite.size;


    // Convert to NDC
    var ndc = (worldPos / screenSize) * 2.0 - vec2f(1.0, 1.0);
    ndc.y = -ndc.y;

    let zNear = 100.0;
    let zFar = -100.0;

    // Map sprite.z from [-100, 100] -> [0, 1] for depth buffer
    let normalizedZ = (sprite.z - zNear) / (zFar - zNear);

    out.position = vec4f(ndc, normalizedZ, 1.0);
    out.color = sprite.color;

    var uv = input.uv;

    // Check horizontal flip: bit 0
    if ((sprite.flags & 1u) != 0u) {
        uv.x = 1.0 - uv.x;
    }

    // Check vertical flip: bit 1
    if ((sprite.flags & 2u) != 0u) {
        uv.y = 1.0 - uv.y;
    }

    out.uv = sprite.uvOffset + uv * sprite.uvScale;

    return out;
}





@group(0) @binding(1)
var spriteTexture: texture_2d<f32>;

@group(0) @binding(2)
var spriteSampler: sampler;

@fragment
fn fs_main(
    @location(0) color: vec3f,
    @location(1) uv: vec2f
) -> @location(0) vec4f {
    let texColor = textureSample(spriteTexture, spriteSampler, uv);
    
    if (texColor.a == 0) {
        discard;
    }
    return vec4f(color, 1.0) * texColor;
    
}



)";

#include <sys/stat.h>

bool fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

namespace Rune
{
    SpriteBatch::SpriteBatch(Rune::Texture* texture, SpriteOrigin origin)
    {
        this->origin = origin;
        objectCount = 0;

        WGPUBufferDescriptor resBufferDesc = {};
        resBufferDesc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        resBufferDesc.size = sizeof(OtherDataUniform);
        resBufferDesc.mappedAtCreation = false;

        resolutionBuffer = wgpuDeviceCreateBuffer(device, &resBufferDesc);

        textureView = texture->textureView;
        sampler = texture->sampler;

        Rune::Log("Loaded texture");

        WGPUBufferDescriptor bufferDesc{};
        bufferDesc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        bufferDesc.size = MAX_SPRITES * sizeof(SpriteData);

        uniformBufferSize = bufferDesc.size;
        bufferDesc.mappedAtCreation = false;
#ifdef __EMSCRIPTEN__
        bufferDesc.label = nullptr;
#else
        bufferDesc.label = {"", 0};
#endif

        uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

        if (!uniformBuffer)
        {
            Rune::Log("uniformBuffer is null!");
            return;
        }

        Rune::Log("Created uniform buffer");

        WGPUBindGroupLayoutEntry bglEntry{};
        bglEntry.binding = 0;
        bglEntry.buffer.type = WGPUBufferBindingType_Uniform;
        bglEntry.visibility = WGPUShaderStage_Vertex;
        bglEntry.buffer.minBindingSize = MAX_SPRITES * sizeof(SpriteData);
        bglEntry.buffer.hasDynamicOffset = false;

        WGPUBindGroupLayoutEntry textureLayoutEntry{};
        textureLayoutEntry.binding = 1;
        textureLayoutEntry.visibility = WGPUShaderStage_Fragment;
        textureLayoutEntry.texture.sampleType = WGPUTextureSampleType_Float;
        textureLayoutEntry.texture.viewDimension = WGPUTextureViewDimension_2D;
        textureLayoutEntry.texture.multisampled = false;

        WGPUBindGroupLayoutEntry samplerLayoutEntry{};
        samplerLayoutEntry.binding = 2;
        samplerLayoutEntry.visibility = WGPUShaderStage_Fragment;
        samplerLayoutEntry.sampler.type = WGPUSamplerBindingType_Filtering;

        WGPUBindGroupLayoutEntry resolutionLayoutEntry{};
        resolutionLayoutEntry.binding = 3;
        resolutionLayoutEntry.visibility = WGPUShaderStage_Vertex;
        resolutionLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
        resolutionLayoutEntry.buffer.hasDynamicOffset = false;
        resolutionLayoutEntry.buffer.minBindingSize = sizeof(OtherDataUniform);

        WGPUBindGroupLayoutEntry entries[4] = {bglEntry, textureLayoutEntry, samplerLayoutEntry, resolutionLayoutEntry};

        WGPUBindGroupLayoutDescriptor bglDesc{};
        bglDesc.entryCount = 4;
        bglDesc.entries = entries;

        bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bglDesc);

        Rune::Log("Created BGL");

        WGPUBindGroupEntry resBindEntry{};
        resBindEntry.binding = 3;
        resBindEntry.buffer = resolutionBuffer;
        resBindEntry.offset = 0;
        resBindEntry.size = sizeof(OtherDataUniform);

        WGPUBindGroupEntry bindEntries[4] = {
            {.binding = 0,
             .buffer = uniformBuffer,
             .offset = 0,
             .size = MAX_SPRITES * sizeof(SpriteData)},
            {.binding = 1,
             .textureView = textureView},
            {.binding = 2,
             .sampler = sampler},
            resBindEntry};

        WGPUBindGroupDescriptor bgDesc{};
        bgDesc.layout = bindGroupLayout;
        bgDesc.entryCount = 4;
        bgDesc.entries = bindEntries;

        bindGroup = wgpuDeviceCreateBindGroup(device, &bgDesc);

        Rune::Log("Created Bind Group");

        OtherDataUniform resData = {
            .screenWidth = (float)windowWidth,
            .screenHeight = (float)windowHeight,
            .spriteOrigin = static_cast<int>(origin),
            .pad = 0};

        wgpuQueueWriteBuffer(queue, resolutionBuffer, 0, &resData, sizeof(resData));

#ifndef __EMSCRIPTEN__
        WGPUShaderModuleDescriptor shaderDesc{};

        WGPUShaderSourceWGSL shaderCodeDesc{};
        shaderCodeDesc.chain.next = nullptr;
        shaderCodeDesc.chain.sType = WGPUSType_ShaderSourceWGSL;

        shaderCodeDesc.code = {shaderSource, strlen(shaderSource)};

        shaderDesc.nextInChain = &shaderCodeDesc.chain;

#else
        WGPUShaderModuleDescriptor shaderDesc{};

        WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
        shaderCodeDesc.chain.next = nullptr;
        shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;

        shaderCodeDesc.code = shaderSource;

        shaderDesc.nextInChain = &shaderCodeDesc.chain;

#endif

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

        Rune::Log("Compiled shader");

        WGPUVertexAttribute attributes[2] = {
            {
                .format = WGPUVertexFormat_Float32x2,
                .offset = 0,
                .shaderLocation = 0 // position
            },
            {
                .format = WGPUVertexFormat_Float32x2,
                .offset = sizeof(float) * 2,
                .shaderLocation = 1 // uv
            }};

        WGPUVertexBufferLayout vertexBufferLayout{};
        vertexBufferLayout.arrayStride = sizeof(float) * 4;
        vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
        vertexBufferLayout.attributeCount = 2;
        vertexBufferLayout.attributes = attributes;

        WGPURenderPipelineDescriptor pipelineDesc{};
        pipelineDesc.nextInChain = nullptr;
        pipelineDesc.vertex = {};
        pipelineDesc.vertex.bufferCount = 1;
        pipelineDesc.vertex.buffers = &vertexBufferLayout;
        pipelineDesc.vertex.module = shaderModule;
#ifndef __EMSCRIPTEN__
        pipelineDesc.vertex.entryPoint = {"vs_main", 7};
#else
        pipelineDesc.vertex.entryPoint = "vs_main";
#endif
        pipelineDesc.vertex.constantCount = 0;
        pipelineDesc.vertex.constants = nullptr;

        pipelineDesc.primitive = {};
        pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
        pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
        pipelineDesc.primitive.cullMode = WGPUCullMode_None;

        WGPUFragmentState fragmentState = {};
        fragmentState.module = shaderModule;
#ifndef __EMSCRIPTEN__
        fragmentState.entryPoint = {"fs_main", 7};
#else
        fragmentState.entryPoint = "fs_main";
#endif
        fragmentState.constantCount = 0;
        fragmentState.constants = nullptr;

        pipelineDesc.fragment = &fragmentState;

        WGPUBlendState blendState{};
        blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;

        WGPUColorTargetState colorTarget{};
        colorTarget.format = caps.formats[0];
        colorTarget.blend = &blendState;
        colorTarget.writeMask = WGPUColorWriteMask_All;

        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTarget;


        WGPUDepthStencilState depthStencilState = {};
        depthStencilState.format = WGPUTextureFormat_Depth24Plus; // Must match your depth texture
        depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;
        depthStencilState.depthCompare = WGPUCompareFunction_Less; // Typical Z-test
        depthStencilState.stencilReadMask = 0xFFFFFFFF;
        depthStencilState.stencilWriteMask = 0xFFFFFFFF;
        depthStencilState.depthBias = 0;
        depthStencilState.depthBiasSlopeScale = 0.0f;
        depthStencilState.depthBiasClamp = 0.0f;
        depthStencilState.stencilFront = {};
        depthStencilState.stencilBack = {};
        depthStencilState.stencilFront.compare = WGPUCompareFunction_Always;
        depthStencilState.stencilFront.failOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilFront.passOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilBack = depthStencilState.stencilFront;

        pipelineDesc.depthStencil = &depthStencilState;
        // Samples per pixel
        pipelineDesc.multisample.count = 1;
        // Default value for the mask, meaning "all bits on"
        pipelineDesc.multisample.mask = ~0u;
        // Default value as well (irrelevant for count = 1 anyways)
        pipelineDesc.multisample.alphaToCoverageEnabled = false;

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
        pipelineLayoutDesc.bindGroupLayoutCount = 1;
        pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;

        pipelineDesc.layout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDesc);

        pipeline = wgpuDeviceCreateRenderPipeline(Rune::device, &pipelineDesc);

        Rune::Log("Created pipeline");

        wgpuShaderModuleRelease(shaderModule);

        // Vertex data for a unit square (0,0)-(1,1)
        float centerQuadVertices[] = {
            // x,    y,     u,    v
            -0.5f, -0.5f, 0.0f, 0.0f, // Bottom Left
            0.5f, -0.5f, 1.0f, 0.0f,  // Bottom Right
            0.5f, 0.5f, 1.0f, 1.0f,   // Top Right

            -0.5f, -0.5f, 0.0f, 0.0f, // Bottom Left
            0.5f, 0.5f, 1.0f, 1.0f,   // Top Right
            -0.5f, 0.5f, 0.0f, 1.0f   // Top Left
        };

        // Vertex data for a unit square (0,0)-(1,1)
        float topLeftQuadVertices[] = {
            //  x,   y,     u,    v
            0.0f, 1.0f,  0.0f, 1.0f,  // Top Left
            1.0f, 1.0f,  1.0f, 1.0f,  // Top Right
            1.0f, 0.0f,  1.0f, 0.0f,  // Bottom Right

            0.0f, 1.0f,  0.0f, 1.0f,  // Top Left
            1.0f, 0.0f,  1.0f, 0.0f,  // Bottom Right
            0.0f, 0.0f,  0.0f, 0.0f   // Bottom Left
        };

        float* selected = NULL;

        const size_t vertexCount = 6;
        const size_t vertexStride = sizeof(float) * 4; // should be 16 bytes (4 floats)
        const size_t vertexDataSize = vertexCount * vertexStride;


        switch (origin) {
            case SpriteOrigin::Center:
                selected = centerQuadVertices;
                break;
            case SpriteOrigin::TopLeft:
                selected = topLeftQuadVertices;
                break;
            default:
                // handle unexpected origin value
                Rune::Log("Invalid origin enum value");
                selected = centerQuadVertices;
                break;
        }


        WGPUBufferDescriptor vbDesc = {};
        vbDesc.size = vertexDataSize;
        vbDesc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
        vbDesc.mappedAtCreation = false;

        Rune::Log("Filing Vertex Buffer");

        vertexBuffer = wgpuDeviceCreateBuffer(device, &vbDesc);
        if (!vertexBuffer)
        {
            Rune::Log("VERTEX BUFFER FAIL");
            exit(-1);
        }
        wgpuQueueWriteBuffer(queue, vertexBuffer, 0, selected, vertexDataSize);

        Rune::Log("Created sprite batch");

        uniformWorking = new SpriteData[MAX_SPRITES];
    }

    void SpriteBatch::Begin()
    {
        objectCount = 0;
        started = true;
    }

    void SpriteBatch::Draw(float x, float y, uint w, uint h, float r, float g, float b, float rotation, float z, uint32_t flags)
    {
        DrawTile(x, y, w, h, r, g, b, 0, 0, 1, 1, rotation, z, flags);
    }

    void SpriteBatch::DrawTile(
        float x, float y, uint w, uint h,
        float r, float g, float b,
        uint tileX, uint tileY,
        uint tilesPerRow, uint tilesPerCol,
        float rotation, float z, uint32_t flags)
    {
        /*if (objectCount == MAX_SPRITES)
        {
            throttledCount += 1;
            return;
        }*/

        SpriteData sprite;
        sprite.pos[0] = x;
        sprite.pos[1] = y;
        sprite.z = z;
        sprite.size[0] = (float)w;
        sprite.size[1] = (float)h;
        sprite.color[0] = r;
        sprite.color[1] = g;
        sprite.color[2] = b;

        float uvTileW = 1.0f / (float)tilesPerRow;
        float uvTileH = 1.0f / (float)tilesPerCol;

        sprite.uvOffset[0] = tileX * uvTileW;
        sprite.uvOffset[1] = tileY * uvTileH;

        sprite.uvScale[0] = uvTileW;
        sprite.uvScale[1] = uvTileH;

        sprite.flags = flags;

        sprite.rotation = rotation;

        int useCount = objectCount;

        if (useCount >= MAX_SPRITES)
        {
            Rune::Log("ANDROIDTEST123 objectCount exceeds MAX_SPRITES!");
            return;
        }

        uniformWorking[useCount] = sprite;

        objectCount++;
    }

    void SpriteBatch::DrawImageRect(
        float x, float y, uint w, uint h,
        float r, float g, float b,
        uint srcX, uint srcY, uint srcW, uint srcH,
        uint textureWidth, uint textureHeight,
        float rotation, float z, uint32_t flags)
    {
        if (objectCount >= MAX_SPRITES)
        {
            Rune::Log("SpriteBatch::DrawImageRect: objectCount exceeds MAX_SPRITES!");
            return;
        }

        SpriteData sprite;
        sprite.pos[0] = x;
        sprite.pos[1] = y;
        sprite.z = z;
        sprite.size[0] = static_cast<float>(w);
        sprite.size[1] = static_cast<float>(h);
        sprite.color[0] = r;
        sprite.color[1] = g;
        sprite.color[2] = b;
        sprite.rotation = rotation;
        sprite.flags = flags;

        // Convert pixel-space rect to UVs
        float u0 = static_cast<float>(srcX) / static_cast<float>(textureWidth);
        float v0 = static_cast<float>(srcY) / static_cast<float>(textureHeight);
        float u1 = static_cast<float>(srcX + srcW) / static_cast<float>(textureWidth);
        float v1 = static_cast<float>(srcY + srcH) / static_cast<float>(textureHeight);

        // Set UV offset and scale (similar to how you do in DrawTile)
        sprite.uvOffset[0] = u0;
        sprite.uvOffset[1] = v0;
        sprite.uvScale[0] = u1 - u0;
        sprite.uvScale[1] = v1 - v0;

        uniformWorking[objectCount++] = sprite;
    }

    void SpriteBatch::End()
    {
        void *data = uniformWorking;

        std::cout << "Sprite count: " << objectCount << std::endl;

        if (throttledCount != 0)
            std::cout << "WARNING: " << throttledCount << " SPRITES THROTTLED" << std::endl;

        if (uniformBuffer)
        {

            size_t writeSize = objectCount * sizeof(SpriteData);
            if (writeSize > uniformBufferSize)
            {
                Rune::Log("ERROR: Trying to write beyond buffer size!");
            }
            wgpuQueueWriteBuffer(queue, uniformBuffer, 0, data, objectCount * sizeof(SpriteData));
        }
        else
        {
            Rune::Log("TRIED TO WRITE TO BUFFER THAT DOESNT EXIST");
            exit(-1);
        }

        bufferCreated = true;

        wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);

        bindGroupCreated = true;

        wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, WGPU_WHOLE_SIZE);
        wgpuRenderPassEncoderDraw(renderPass, 6, objectCount, 0, 0);

        started = false;
    }

    void SpriteBatch::Destroy()
    {
        delete[] uniformWorking;
    }
}