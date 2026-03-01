//
// Created by Gabri on 2/27/2026.
//

#include "VK_Context.h"

#include <thread>

#include "VK_ComputeInstance.h"
#include "VK_ComputePipeline.h"
#include "VK_RasterizationPipeline.h"
#include "VK_RasterizationPipelineInstance.h"
#include "VK_RenderPass.h"
#include "VK_RenderTarget.h"
#include "VK_StorageBuffer.h"
#include "VK_TexelBuffer.h"
#include "VK_ValidationLayers.h"
#include "core/Application.h"
#include "core/Configs.h"
#include "raytracing/VK_AABBBottomLevelAccelerationStructure.h"
#include "raytracing/VK_MeshBottomLevelAccelerationStructure.h"
#include "raytracing/VK_RaytracingPipeline.h"
#include "raytracing/VK_RaytracingPipelineInstance.h"
#include "raytracing/VK_TopLevelAccelerationStructure.h"

namespace HBE
{
    VK_Context::VK_Context(ContextInfo& info)
    {
        instance.init(info);
        surface.init(instance, Application::instance->getWindow()->getHandle());
        physical_device.init(instance, surface);
        device.init(physical_device);
        allocator.init(device);
        swapchain.init(device, surface);
        renderer.init(device, swapchain);

        graphic_limits = renderer.getLimits();
    }

    Renderer* VK_Context::getRenderer()
    {
        return &renderer;
    }

    VK_Context::~VK_Context()
    {
        swapchain.release();
        allocator.release();
        device.release();
        physical_device.release();
        surface.release();
        surface.release();
        instance.release();
    }

    GraphicLimits VK_Context::getGraphicLimits() const
    {
        return graphic_limits;
    }

    Image* VK_Context::createImage(const ImageInfo& info)
    {
        return new VK_Image(&device, info);
    }

    RasterizationPipeline* VK_Context::createRasterizationPipeline(const RasterizationPipelineInfo& info)
    {
        return new VK_RasterizationPipeline(&device, &renderer, info);
    }

    Shader* VK_Context::createShader(const ShaderInfo& info)
    {
        return new VK_Shader(&device, info);
    }

    Mesh* VK_Context::createMesh(const MeshInfo& info)
    {
        return new VK_Mesh(&renderer, info);
    }

    ComputePipeline* VK_Context::createComputePipeline(const ComputePipelineInfo& info)
    {
        return new VK_ComputePipeline(&renderer, info);
    }

    RasterizationTarget* VK_Context::createRenderTarget(const RenderTargetInfo& info)
    {
        return new VK_RenderPass(&renderer, info);
    }

    RasterizationPipelineInstance* VK_Context::createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo& info)
    {
        return new VK_RasterizationPipelineInstance(&renderer, info);
    }

    ComputeInstance* VK_Context::createComputeInstance(const ComputeInstanceInfo& info)
    {
        return new VK_ComputeInstance(&renderer, info);
    }

    RootAccelerationStructure* VK_Context::createRootAccelerationStructure(const RootAccelerationStructureInfo& info)
    {
        return new VK_TopLevelAccelerationStructure(&device, info);
    }

    AABBAccelerationStructure* VK_Context::createAABBAccelerationStructure(const AABBAccelerationStructureInfo& info)
    {
        return new VK_AABBBottomLevelAccelerationStructure(&device, info);
    }

    MeshAccelerationStructure* VK_Context::createMeshAccelerationStructure(const MeshAccelerationStructureInfo& info)
    {
        return new VK_MeshBottomLevelAccelerationStructure(&device, info);
    }

    RaytracingPipeline* VK_Context::createRaytracingPipeline(const RaytracingPipelineInfo& info)
    {
        return new VK_RaytracingPipeline(&renderer, info);
    }

    RaytracingPipelineInstance* VK_Context::createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo& info)
    {
        return new VK_RaytracingPipelineInstance(renderer, info);
    }

    StorageBuffer* VK_Context::createStorageBuffer(const StorageBufferInfo& info)
    {
        return new VK_StorageBuffer(&device, info);
    }


    TexelBuffer* VK_Context::createTexelBuffer(const TexelBufferInfo& info)
    {
        return new VK_TexelBuffer(&device, info);
    }

    Font* VK_Context::createFont(const FontInfo& font_info) const
    {
        return new Font(font_info);
    }
}
