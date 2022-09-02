#include "VK_ResourceFactory.h"
#include "VK_GraphicPipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Image.h"
#include "VK_CommandPool.h"
#include "VK_Mesh.h"
#include "VK_Shader.h"
#include "VK_RenderPass.h"
#include "VK_ComputePipeline.h"
#include "VK_Material.h"
#include "VK_ComputeInstance.h"
#include "raytracing/VK_TopLevelAccelerationStructure.h"
#include "raytracing/VK_AABBBottomLevelAccelerationStructure.h"
#include "raytracing/VK_MeshBottomLevelAccelerationStructure.h"

namespace HBE {
	VK_ResourceFactory::VK_ResourceFactory(VK_Renderer *renderer) {
		this->renderer = renderer;
	}

	GraphicPipeline *VK_ResourceFactory::createGraphicPipeline(const GraphicPipelineInfo &info) const {
		return new VK_GraphicPipeline(renderer->getDevice(), renderer, info);
	}

	Shader *VK_ResourceFactory::createShader(const ShaderInfo &info) const {
		return new VK_Shader(renderer->getDevice(), info);
	}

	Texture *VK_ResourceFactory::createTexture(const TextureInfo &info) const {
		return new VK_Image(renderer->getDevice(), info);
	}

	Mesh *VK_ResourceFactory::createMesh(const MeshInfo &info) const {
		return new VK_Mesh(renderer, renderer->getCommandPool(), info);
	}

	ComputePipeline *VK_ResourceFactory::createComputePipeline(const ComputePipelineInfo &info) const {
		return new VK_ComputePipeline(renderer, info);
	}

	RenderTarget *VK_ResourceFactory::createRenderTarget(const RenderTargetInfo &info) const {
		return new VK_RenderPass(renderer, info);
	}

	Material *VK_ResourceFactory::createMaterial(const MaterialInfo &info) const {
		return new VK_Material(renderer, info);
	}

	ComputeInstance *VK_ResourceFactory::createComputeInstance(const ComputeInstanceInfo &info) const {
		return new VK_ComputeInstance(renderer, info);
	}

	RootAccelerationStructure *VK_ResourceFactory::createRootAccelerationStructure(const RootAccelerationStructureInfo &info) const {
		return new VK_TopLevelAccelerationStructure(renderer->getDevice(), info);
	}

	AABBAccelerationStructure *VK_ResourceFactory::createAABBAccelerationStructure(const AABBAccelerationStructureInfo &info) const {
		return new VK_AABBBottomLevelAccelerationStructure(renderer->getDevice(), info);
	}

	MeshAccelerationStructure *VK_ResourceFactory::createMeshAccelerationStructure(const MeshAccelerationStructureInfo &info) const {
		return new VK_MeshBottomLevelAccelerationStructure(renderer->getDevice(), info);
	}

	RaytracingPipeline *VK_ResourceFactory::createRaytracingPipeline(const RaytracingPipelineInfo &info) const {
		return new VK_RaytracingPipeline(renderer, info);
	}

}

