#pragma once
#include "VK_Device.h"
#include "VK_Image.h"
#include "VK_Instance.h"
#include "VK_Renderer.h"
#include "VK_Surface.h"
#include "VK_Swapchain.h"
#include "VK_ValidationLayers.h"
#include "core/Context.h"

namespace HBE
{
    class VK_Context : public Context
    {
        VK_Instance instance;


    private:
        VK_Surface surface;
        VK_PhysicalDevice physical_device;
        VK_Device device;
        VK_Allocator allocator;
        VK_Swapchain swapchain;

        VK_Renderer renderer;

        GraphicLimits graphic_limits;

    public:
        Renderer* getRenderer() override;

        explicit VK_Context(ContextInfo& info);

        ~VK_Context() override;
        GraphicLimits getGraphicLimits() const override;
        Image* createImage(const ImageInfo& info) override;
        RasterizationPipeline* createRasterizationPipeline(const RasterizationPipelineInfo& info) override;
        Shader* createShader(const ShaderInfo& info) override;
        Mesh* createMesh(const MeshInfo& info) override;
        ComputePipeline* createComputePipeline(const ComputePipelineInfo& info) override;
        RasterizationTarget* createRenderTarget(const RenderTargetInfo& info) override;
        RasterizationPipelineInstance* createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo& info) override;
        ComputeInstance* createComputeInstance(const ComputeInstanceInfo& info) override;
        RootAccelerationStructure* createRootAccelerationStructure(const RootAccelerationStructureInfo& info) override;
        AABBAccelerationStructure* createAABBAccelerationStructure(const AABBAccelerationStructureInfo& info) override;
        MeshAccelerationStructure* createMeshAccelerationStructure(const MeshAccelerationStructureInfo& info) override;
        RaytracingPipeline* createRaytracingPipeline(const RaytracingPipelineInfo& info) override;
        RaytracingPipelineInstance* createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo& info) override;
        StorageBuffer* createStorageBuffer(const StorageBufferInfo& info) override;
        TexelBuffer* createTexelBuffer(const TexelBufferInfo& info) override;
        Font* createFont(const FontInfo& font_info) const override;
    };
}
