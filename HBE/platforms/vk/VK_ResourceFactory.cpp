#include "VK_ResourceFactory.h"
#include "VK_GraphicPipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Image.h"
#include "VK_CommandPool.h"
#include "VK_Mesh.h"
#include "VK_Shader.h"
#include "VK_VertexLayout.h"

namespace HBE {
    VK_ResourceFactory::VK_ResourceFactory(VK_Renderer *renderer) {
        this->renderer = renderer;
    }

    GraphicPipeline *VK_ResourceFactory::createGraphicsPipeline() const {
        return new VK_GraphicPipeline(renderer->getDevice(), renderer);
    }

    Texture *VK_ResourceFactory::createTexture() const {
        return new VK_Image(renderer->getDevice());
    }

    Mesh *VK_ResourceFactory::createMesh() const {
        return new VK_Mesh(renderer->getDevice(), renderer->getCommandPool());
    }

	Shader *VK_ResourceFactory::createShader() const {
		return new VK_Shader(renderer->getDevice());
	}

    ComputePipeline *VK_ResourceFactory::createComputePipeline() const {
        Log::error("Compute shaders not implemented in Vulkan");
        return nullptr;
    }

    VertexLayout *VK_ResourceFactory::createVertexLayout() const {
        return new VK_VertexLayout();
    }


}

