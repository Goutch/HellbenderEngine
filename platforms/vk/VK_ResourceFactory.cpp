#include "VK_ResourceFactory.h"
#include "VK_RasterizationPipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Image.h"
#include "VK_CommandPool.h"
#include "VK_Mesh.h"
#include "VK_Shader.h"
#include "VK_RenderPass.h"
#include "VK_ComputePipeline.h"
#include "VK_RasterizationPipelineInstance.h"
#include "VK_ComputeInstance.h"
#include "raytracing/VK_TopLevelAccelerationStructure.h"
#include "raytracing/VK_AABBBottomLevelAccelerationStructure.h"
#include "raytracing/VK_MeshBottomLevelAccelerationStructure.h"
#include "raytracing/VK_RaytracingPipeline.h"
#include "raytracing/VK_RaytracingPipelineInstance.h"
#include "VK_StorageBuffer.h"
#include "VK_TexelBuffer.h"
#include "VK_DynamicRenderTarget.h"

namespace HBE {
	VK_ResourceFactory::VK_ResourceFactory(VK_Renderer *renderer) {
		this->renderer = renderer;
	}

	RasterizationPipeline *VK_ResourceFactory::createRasterizationPipeline(const RasterizationPipelineInfo &info) const {
		return new VK_RasterizationPipeline(renderer->getDevice(), renderer, info);
	}

	Shader *VK_ResourceFactory::createShader(const ShaderInfo &info) const {
		return new VK_Shader(renderer->getDevice(), info);
	}

	Image *VK_ResourceFactory::createImage(const ImageInfo &info) const {
		return new VK_Image(renderer->getDevice(), info);
	}

	Mesh *VK_ResourceFactory::createMesh(const MeshInfo &info) const {
		return new VK_Mesh(renderer, renderer->getCommandPool(), info);
	}

	ComputePipeline *VK_ResourceFactory::createComputePipeline(const ComputePipelineInfo &info) const {
		return new VK_ComputePipeline(renderer, info);
	}

	RasterizationTarget *VK_ResourceFactory::createRenderTarget(const RenderTargetInfo &info) const {
		//use dynamic rendering if the device supports it
		if(renderer->getDevice()->getPhysicalDevice().getEnabledExtensionFlags() & EXTENSION_FLAG_DYNAMIC_RENDERING)
		{
			return new VK_RenderPass(renderer, info);
			//return new VK_DynamicRenderTarget(renderer, info);
		}
		else
		{
			return new VK_RenderPass(renderer, info);
		}
	}

	RasterizationPipelineInstance *VK_ResourceFactory::createRasterizationPipelineInstance(const RasterizationPipelineInstanceInfo &info) const {
		return new VK_RasterizationPipelineInstance(renderer, info);
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

	RaytracingPipelineInstance *VK_ResourceFactory::createRaytracingPipelineInstance(const RaytracingPipelineInstanceInfo &info) const {
		return new VK_RaytracingPipelineInstance(*renderer, info);
	}

	StorageBuffer *VK_ResourceFactory::createStorageBuffer(const StorageBufferInfo &info) const {
		return new VK_StorageBuffer(renderer->getDevice(), info);
	}

	TexelBuffer *VK_ResourceFactory::createTexelBuffer(const TexelBufferInfo &info) const {
		return new VK_TexelBuffer(renderer->getDevice(), info);
	}

}


