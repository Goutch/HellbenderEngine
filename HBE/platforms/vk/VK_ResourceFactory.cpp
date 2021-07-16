#include "VK_ResourceFactory.h"
#include "VK_GraphicPipeline.h"
#include "VK_Texture.h"
#include "VK_Mesh.h"
#include "VK_Shader.h"
#include "VK_VertexLayout.h"
namespace HBE {

    VK_ResourceFactory::VK_ResourceFactory(const VK_Device *device, const VK_Renderer *renderer) {
        this->device = device;
        this->renderer = renderer;
    }

    GraphicPipeline *VK_ResourceFactory::createGraphicsPipeline() const {
        return new VK_GraphicPipeline(device, renderer);
    }

    Texture *VK_ResourceFactory::createTexture() const {
        return new VK_Texture(device);
    }

    Mesh *VK_ResourceFactory::createMesh() const {
        return new VK_Mesh(device);
    }

    Shader *VK_ResourceFactory::createShader() const {
        return new VK_Shader(device);
    }

    ComputePipeline *VK_ResourceFactory::createComputePipeline() const {
        Log::error("Compute shaders not implemented in Vulkan");
        return nullptr;
    }

    VertexLayout *VK_ResourceFactory::createVertexLayout() const {
        return new VK_VertexLayout();
    }
}

