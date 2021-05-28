#include "VK_ResourceFactory.h"
#include "VK_GraphicPipeline.h"
#include "VK_Texture.h"
#include "VK_Mesh.h"
#include "VK_Shader.h"
#include "VK_Device.h"

namespace HBE {

    VK_ResourceFactory::VK_ResourceFactory(const VK_Device *device, const VK_Renderer *renderer) {
        this->device = device;
        this->renderer = renderer;
    }

    IGraphicPipeline *VK_ResourceFactory::createGraphicsPipeline() const {
        return new VK_GraphicPipeline(device,renderer);
    }

    ITexture *VK_ResourceFactory::createTexture() const {
        return new VK_Texture(device);
    }

    IMesh *VK_ResourceFactory::createMesh() const {
        return new VK_Mesh(device);
    }

    IShader *VK_ResourceFactory::createShader() const {
        return new VK_Shader(device);
    }

    IComputePipeline *VK_ResourceFactory::createComputePipeline() const {
        Log::error("Compute shaders not implemented in Vulkan");
        return nullptr;
    }


}

