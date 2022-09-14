#pragma once

#include "core/resource/ResourceFactory.h"

namespace HBE {

    class VK_Renderer;


    class VK_ResourceFactory : public ResourceFactory {
        VK_Renderer* renderer;
    public:
        VK_ResourceFactory(VK_Renderer *renderer);
		GraphicPipeline *createGraphicPipeline(const GraphicPipelineInfo &info) const override;
		Shader *createShader(const ShaderInfo &info) const override;
		Texture *createTexture(const TextureInfo &info) const override;
		Mesh *createMesh(const MeshInfo &info) const override;
		ComputePipeline *createComputePipeline(const ComputePipelineInfo &info) const override;
		RenderTarget *createRenderTarget(const RenderTargetInfo &info) const override;
		Material *createMaterial(const MaterialInfo &info) const override;
		ComputeInstance *createComputeInstance(const ComputeInstanceInfo &info) const override;
		RootAccelerationStructure *createRootAccelerationStructure(const RootAccelerationStructureInfo &info) const override;
		AABBAccelerationStructure *createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info) const override;
		MeshAccelerationStructure *createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info) const override;
		RaytracingPipeline *createRaytracingPipeline(const RaytracingPipelineInfo &info) const override;
		RaytracingPipelineInstance *createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info) const override;
	};
}

