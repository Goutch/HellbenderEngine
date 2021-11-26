#include "VK_ResourceFactory.h"
#include "VK_GraphicPipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Image.h"
#include "VK_CommandPool.h"
#include "VK_Mesh.h"
#include "VK_Shader.h"
#include "VK_VertexLayout.h"
#include "VK_RenderPass.h"

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
		Log::error("Compute shaders not implemented in Vulkan");
		return nullptr;
	}

	RenderTarget *VK_ResourceFactory::createRenderTarget(const RenderTargetInfo &info) const {
		return new VK_RenderPass(renderer, info);
	}

}

