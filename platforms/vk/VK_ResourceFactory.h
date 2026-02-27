#pragma once

#include "core/resource/ResourceFactory.h"
#include "VK_Image.h"
namespace HBE {

    class VK_Renderer;

	struct VK_ImageData;
    class VK_ResourceFactory : public ResourceFactory {
		std::vector<VK_ImageData> images;
        VK_Renderer* renderer;
    public:
        VK_ResourceFactory(VK_Renderer *renderer);
		RasterizationPipeline *createRasterizationPipeline(const RasterizationPipelineInfo &info) const override;
		Shader *createShader(const ShaderInfo &info) const override;
		ImageHandle createImage(const ImageInfo &info) const override;
		Mesh *createMesh(const MeshInfo &info) const override;
		ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) const override;
		RasterizationTarget *createRenderTarget(const RenderTargetInfo &info) const override;
		RasterizationPipelineInstance *createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info) const override;
		ComputeInstance *createComputeInstance(const ComputeInstanceInfo &info) const override;
		RootAccelerationStructure *createRootAccelerationStructure(const RootAccelerationStructureInfo &info) const override;
		AABBAccelerationStructure *createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info) const override;
		MeshAccelerationStructure *createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info) const override;
		RaytracingPipeline *createRaytracingPipeline(const RaytracingPipelineInfo &info) const override;
		RaytracingPipelineInstance *createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info) const override;

		StorageBuffer *createStorageBuffer(const StorageBufferInfo &info) const override;
		TexelBuffer* createTexelBuffer(const TexelBufferInfo& info) const override;
	};
}

