#include <Rune/GeometryRenderer.hpp>
#include <cstring>

const char *geometryShaderSource = R"(
struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color: vec3f,
    @location(2) uv: vec2f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
    @location(1) uv: vec2f
};

@group(0) @binding(0)
var<uniform> screenSize: vec2f;

@group(0) @binding(1)
var myTexture: texture_2d<f32>;

@group(0) @binding(2)
var mySampler: sampler;

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    let ndc = (input.position / screenSize) * 2.0 - vec2f(1.0, 1.0);
    out.position = vec4f(ndc.x, -ndc.y, 0.0, 1.0);
    out.color = input.color;
    out.uv = input.uv;
    return out;
}

@fragment
fn fs_main(@location(0) color: vec3f, @location(1) uv: vec2f) -> @location(0) vec4f {
    let texColor = textureSample(myTexture, mySampler, uv);
    // Multiply vertex color with texture color
    return vec4f(color * texColor.rgb, texColor.a);
}



)";

namespace Rune
{
    GeometryRenderer::GeometryRenderer(Rune::Texture *texture)
    {
        if (texture == nullptr) {
            char data[] = {
                (char)255, (char)255, (char)255, (char)255,
                (char)255, (char)255, (char)255, (char)255,
                (char)255, (char)255, (char)255, (char)255,
                (char)255, (char)255, (char)255, (char)255
            };
            this->texture = new Texture(1, 1, 4, data, Rune::SamplingMode::Linear);
        } else {
            this->texture = texture;
        }

        // Create screenSize uniform buffer
        WGPUBufferDescriptor screenDesc = {};
        screenDesc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        screenDesc.size = sizeof(float) * 2;
        screenDesc.mappedAtCreation = false;
        screenDesc.label = Rune::CreateString("Geometry Renderer Resolution Buffer");
        screenSizeBuffer = wgpuDeviceCreateBuffer(device, &screenDesc);

        // Bind group layout
        WGPUBindGroupLayoutEntry layoutEntries[3] = {};

        // Uniform buffer binding (screenSize)
        layoutEntries[0].binding = 0;
        layoutEntries[0].visibility = WGPUShaderStage_Vertex;
        layoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
        layoutEntries[0].buffer.hasDynamicOffset = false;
        layoutEntries[0].buffer.minBindingSize = sizeof(float) * 2;

        // Texture binding
        layoutEntries[1].binding = 1;
        layoutEntries[1].visibility = WGPUShaderStage_Fragment;
        layoutEntries[1].texture.sampleType = WGPUTextureSampleType_Float;
        layoutEntries[1].texture.viewDimension = WGPUTextureViewDimension_2D;
        layoutEntries[1].texture.multisampled = false;

        // Sampler binding
        layoutEntries[2].binding = 2;
        layoutEntries[2].visibility = WGPUShaderStage_Fragment;
        layoutEntries[2].sampler.type = WGPUSamplerBindingType_Filtering;

        WGPUBindGroupLayoutDescriptor bglDesc = {};
        bglDesc.entryCount = 3;
        bglDesc.entries = layoutEntries;
        bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bglDesc);

        WGPUBindGroupEntry bgEntries[3] = {};

        // Screen size uniform buffer
        bgEntries[0].binding = 0;
        bgEntries[0].buffer = screenSizeBuffer;
        bgEntries[0].offset = 0;
        bgEntries[0].size = sizeof(float) * 2;

        // Texture
        bgEntries[1].binding = 1;
        bgEntries[1].textureView = this->texture->textureView;

        // Sampler
        bgEntries[2].binding = 2;
        bgEntries[2].sampler = this->texture->sampler;

        WGPUBindGroupDescriptor bgDesc = {};
        bgDesc.layout = bindGroupLayout;
        bgDesc.entryCount = 3;
        bgDesc.entries = bgEntries;

        bindGroup = wgpuDeviceCreateBindGroup(device, &bgDesc);

        CreatePipeline();
    }

    void GeometryRenderer::CreatePipeline()
    {
        // Load shader

#ifndef __EMSCRIPTEN__
        WGPUShaderSourceWGSL shaderCodeDesc{};
        shaderCodeDesc.chain.sType = WGPUSType_ShaderSourceWGSL;
        shaderCodeDesc.code = {geometryShaderSource, strlen(geometryShaderSource)};
#else
        WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
        shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        shaderCodeDesc.code = geometryShaderSource;
#endif

        WGPUShaderModuleDescriptor shaderDesc{};
        shaderDesc.nextInChain = &shaderCodeDesc.chain;
        shaderDesc.label = Rune::CreateString("Geometry Renderer Shader");

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

        // Vertex layout
        WGPUVertexAttribute attributes[3] = {
            {.format = WGPUVertexFormat_Float32x2, .offset = 0, .shaderLocation = 0}, // position
            {.format = WGPUVertexFormat_Float32x3, .offset = sizeof(float) * 2, .shaderLocation = 1},
            {.format = WGPUVertexFormat_Float32x2, .offset = sizeof(float) * 5, .shaderLocation = 2} // color
        };

        WGPUVertexBufferLayout vertexLayout = {};
        vertexLayout.arrayStride = sizeof(float) * 7;
        vertexLayout.stepMode = WGPUVertexStepMode_Vertex;
        vertexLayout.attributeCount = 3;
        vertexLayout.attributes = attributes;

        // Fragment
        WGPUBlendState blendState{};
        blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;

        WGPUColorTargetState colorTarget = {};
        colorTarget.format = caps.formats[0];
        colorTarget.blend = &blendState;
        colorTarget.writeMask = WGPUColorWriteMask_All;

        WGPUFragmentState fragment = {};
        fragment.module = shaderModule;
#ifndef __EMSCRIPTEN__
        fragment.entryPoint = {"fs_main", 7};
#else
        fragment.entryPoint = "fs_main";
#endif
        fragment.targetCount = 1;
        fragment.targets = &colorTarget;

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
        pipelineLayoutDesc.bindGroupLayoutCount = 1;
        pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDesc);

        WGPURenderPipelineDescriptor pipelineDesc{};
        pipelineDesc.layout = pipelineLayout;
        pipelineDesc.vertex.module = shaderModule;
#ifndef __EMSCRIPTEN__
        pipelineDesc.vertex.entryPoint = {"vs_main", 7};
#else
        pipelineDesc.vertex.entryPoint = "vs_main";
#endif
        pipelineDesc.vertex.bufferCount = 1;
        pipelineDesc.vertex.buffers = &vertexLayout;

        pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        pipelineDesc.primitive.cullMode = WGPUCullMode_None;
        pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;

        pipelineDesc.fragment = &fragment;
        pipelineDesc.multisample.count = 1;
        pipelineDesc.multisample.mask = ~0u;
        pipelineDesc.multisample.alphaToCoverageEnabled = false;


        WGPUDepthStencilState depthStencilState = {};
        depthStencilState.format = WGPUTextureFormat_Depth24Plus; // Must match your depth texture
        depthStencilState.depthWriteEnabled = WGPUOptionalBool_False;
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

        pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);
    }

    void GeometryRenderer::SetTexture(Rune::Texture *newTexture)
    {
        if (!newTexture || !newTexture->textureView || !newTexture->sampler)
        {
            Log("GeometryRenderer::SetTexture received invalid texture — skipping.");
            return;
        }

        texture = newTexture;

        // Destroy old bind group if necessary
        if (bindGroup)
        {
            wgpuBindGroupRelease(bindGroup); // or wgpuBindGroupDestroy if available
            bindGroup = nullptr;
        }

        // Create new bind group with updated texture and sampler
        WGPUBindGroupEntry bgEntries[3] = {};

        bgEntries[0].binding = 0;
        bgEntries[0].buffer = screenSizeBuffer;
        bgEntries[0].offset = 0;
        bgEntries[0].size = sizeof(float) * 2;

        bgEntries[1].binding = 1;
        bgEntries[1].textureView = texture->textureView;

        bgEntries[2].binding = 2;
        bgEntries[2].sampler = texture->sampler;

        WGPUBindGroupDescriptor bgDesc = {};
        bgDesc.layout = bindGroupLayout;
        bgDesc.entryCount = 3;
        bgDesc.entries = bgEntries;

        bindGroup = wgpuDeviceCreateBindGroup(device, &bgDesc);
    }

    void GeometryRenderer::SetVertices(const std::vector<float> &vertices)
    {
        vertexCount = vertices.size() / 7;

        // Instead of destroying old buffer, just create a new one
        WGPUBufferDescriptor vbDesc = {};
        vbDesc.size = vertices.size() * sizeof(float);
        vbDesc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
        vbDesc.label = Rune::CreateString("Geometry Renderer Vertex Buffer");

        vertexBuffer = wgpuDeviceCreateBuffer(device, &vbDesc);

        wgpuQueueWriteBuffer(queue, vertexBuffer, 0, vertices.data(), vbDesc.size);
    }


    void GeometryRenderer::Draw()
    {
        if (vertexBuffer) {
            float screenData[2] = {(float)windowWidth, (float)windowHeight};
            wgpuQueueWriteBuffer(queue, screenSizeBuffer, 0, screenData, sizeof(screenData));

            wgpuRenderPassEncoderSetBindGroup(activeRenderPass, 0, bindGroup, 0, nullptr);
            wgpuRenderPassEncoderSetPipeline(activeRenderPass, pipeline);
            wgpuRenderPassEncoderSetVertexBuffer(activeRenderPass, 0, vertexBuffer, 0, WGPU_WHOLE_SIZE);
            wgpuRenderPassEncoderDraw(activeRenderPass, vertexCount, 1, 0, 0);
        }
        
    }

    GeometryRenderer::~GeometryRenderer()
    {
        if (vertexBuffer)
            wgpuBufferDestroy(vertexBuffer);
        if (screenSizeBuffer)
            wgpuBufferDestroy(screenSizeBuffer);
    }
}