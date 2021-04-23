//
// Created by user on 4/19/2021.
//

#include "GL_ResourceFactory.h"
#include "GL_GraphicPipeline.h"
#include "GL_Texture.h"
#include "GL_Mesh.h"
#include "GL_Shader.h"
#include "core/utility/Log.h"
namespace HBE{
    IGraphicPipeline *HBE::GL_ResourceFactory::createGraphicsPipeline() const {
        return new GL_GraphicPipeline();
    }

    ITexture *HBE::GL_ResourceFactory::createTexture() const {
        return new GL_Texture();
    }

    IMesh *HBE::GL_ResourceFactory::createMesh() const {
        return new GL_Mesh();
    }

    HBE::IShader *HBE::GL_ResourceFactory::createShader() const {
        return new GL_Shader();
    }

    IComputePipeline *GL_ResourceFactory::createComputePipeline() const {
        Log::error("Compute shader");
        return nullptr;
    }
}

