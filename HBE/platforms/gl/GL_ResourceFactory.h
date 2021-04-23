#pragma once

#include <core/resource/GraphicPipeline.h>
#include <core/resource/Texture.h>
#include "core/resource/IResourceFactory.h"

namespace HBE {
    class GL_ResourceFactory : public IResourceFactory {
    public:
        virtual ~GL_ResourceFactory(){}
        IGraphicPipeline *createGraphicsPipeline() const override;

        ITexture *createTexture() const override;

        IMesh *createMesh() const override;

        IComputePipeline *createComputePipeline() const override;

        IShader *createShader() const override;
    };
}




