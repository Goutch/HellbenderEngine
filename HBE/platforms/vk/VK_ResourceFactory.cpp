#include "VK_ResourceFactory.h"
#include "VK_GraphicPipeline.h"
#include "VK_Shader.h"
namespace HBE {

    VK_ResourceFactory::VK_ResourceFactory(VK_Device *device) {
        this->device = device;
    }

    GraphicPipeline *HBE::VK_ResourceFactory::createGraphicsPipeline() const {
        return new VK_GraphicPipeline(device);
    }

    Texture *HBE::VK_ResourceFactory::createTexture() const {
        return new VK_Texture(device);
    }

    Mesh *HBE::VK_ResourceFactory::createMesh() const {
        return new VK_Mesh(device);
    }

    IShader *VK_ResourceFactory::createShader() const {
        return new VK_Shader();
    }


}

