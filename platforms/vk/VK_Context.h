#pragma once
#include "VK_Allocator.h"
#include "VK_Device.h"
#include "VK_Image.h"
#include "VK_Instance.h"
#include "VK_Renderer.h"
#include "VK_Surface.h"
#include "VK_Swapchain.h"
#include "core/Context.h"

namespace HBE
{
    class Allocator;

    class VK_Context : public Context
    {
    public:
        VK_Instance instance;
        VK_Surface surface;
        VK_PhysicalDevice physical_device;
        VK_Device device;
        VK_Allocator allocator;
        VK_Swapchain swapchain;
        VK_Renderer renderer;
        GraphicLimits graphic_limits;


        explicit VK_Context(ContextInfo& info);

        ~VK_Context() final;

        Renderer* getRenderer() final;
        GraphicLimits getGraphicLimits() const final;
        Image* createImage(const ImageInfo& info) final;
        RasterizationPipeline* createRasterizationPipeline(const RasterizationPipelineInfo& info) final;
        Shader* createShader(const ShaderInfo& info) final;
        Mesh* createMesh(const MeshInfo& info) final;
        ComputePipeline* createComputePipeline(const ComputePipelineInfo& info) final;
        RasterizationTarget* createRenderTarget(const RenderTargetInfo& info) final;
        RasterizationPipelineInstance* createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo& info) final;
        ComputeInstance* createComputeInstance(const ComputeInstanceInfo& info) final;
        RootAccelerationStructure* createRootAccelerationStructure(const RootAccelerationStructureInfo& info) final;
        AABBAccelerationStructure* createAABBAccelerationStructure(const AABBAccelerationStructureInfo& info) final;
        MeshAccelerationStructure* createMeshAccelerationStructure(const MeshAccelerationStructureInfo& info) final;
        RaytracingPipeline* createRaytracingPipeline(const RaytracingPipelineInfo& info) final;
        RaytracingPipelineInstance* createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo& info) final;
        StorageBuffer* createStorageBuffer(const StorageBufferInfo& info) final;
        TexelBuffer* createTexelBuffer(const TexelBufferInfo& info) final;
        Font* createFont(const FontInfo& font_info) const final;
    };
}
