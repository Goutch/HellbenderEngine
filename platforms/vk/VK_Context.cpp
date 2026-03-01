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
        device.init(this);
        allocator.init(this);
        swapchain.init(this);


        renderer.init(this);
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
        VK_Image* image = new VK_Image();
        image->alloc(this, info);
        return image;
    }

    RasterizationPipeline* VK_Context::createRasterizationPipeline(const RasterizationPipelineInfo& info)
    {
        if (info.rasterization_target == nullptr)
            info.rasterization_target = dynamic_cast<RasterizationTarget*>(renderer.getDefaultRenderTarget());
        VK_RasterizationPipeline* raster = new VK_RasterizationPipeline();
        raster->init(this, info);
        return raster;
    }

    Shader* VK_Context::createShader(const ShaderInfo& info)
    {
        return new VK_Shader(this, info);
    }

    Mesh* VK_Context::createMesh(const MeshInfo& info)
    {
        return new VK_Mesh(this, info);
    }

    ComputePipeline* VK_Context::createComputePipeline(const ComputePipelineInfo& info)
    {
        return new VK_ComputePipeline(this, info);
    }

    RasterizationTarget* VK_Context::createRenderTarget(const RenderTargetInfo& info)
    {
        return new VK_RenderPass(this, info);
    }

    RasterizationPipelineInstance* VK_Context::createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo& info)
    {
        return new VK_RasterizationPipelineInstance(this, info);
    }

    ComputeInstance* VK_Context::createComputeInstance(const ComputeInstanceInfo& info)
    {
        return new VK_ComputeInstance(this, info);
    }

    RootAccelerationStructure* VK_Context::createRootAccelerationStructure(const RootAccelerationStructureInfo& info)
    {
        return new VK_TopLevelAccelerationStructure(this, info);
    }

    AABBAccelerationStructure* VK_Context::createAABBAccelerationStructure(const AABBAccelerationStructureInfo& info)
    {
        return new VK_AABBBottomLevelAccelerationStructure(this, info);
    }

    MeshAccelerationStructure* VK_Context::createMeshAccelerationStructure(const MeshAccelerationStructureInfo& info)
    {
        return new VK_MeshBottomLevelAccelerationStructure(this, info);
    }

    RaytracingPipeline* VK_Context::createRaytracingPipeline(const RaytracingPipelineInfo& info)
    {
        return new VK_RaytracingPipeline(this, info);
    }

    RaytracingPipelineInstance* VK_Context::createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo& info)
    {
        return new VK_RaytracingPipelineInstance(this, info);
    }

    StorageBuffer* VK_Context::createStorageBuffer(const StorageBufferInfo& info)
    {
        //todo fix this.
        return nullptr; //new VK_StorageBuffer(this, info);
    }


    TexelBuffer* VK_Context::createTexelBuffer(const TexelBufferInfo& info)
    {
        VK_TexelBuffer* texel_buffer = new VK_TexelBuffer();;
        texel_buffer->alloc(this, info);
        return texel_buffer;
    }

    Font* VK_Context::createFont(const FontInfo& font_info) const
    {
        return new Font(font_info);
    }
}
