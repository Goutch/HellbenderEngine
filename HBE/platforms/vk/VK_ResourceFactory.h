#pragma once

#include <core/resource/Material.h>
#include <core/resource/Model.h>
#include "core/resource/IResourceFactory.h"
#include "VK_Device.h"

namespace HBE {
    class VK_ResourceFactory : public IResourceFactory {
        VK_Device *device;
    public:
        VK_ResourceFactory(VK_Device *device);

        IGraphicPipeline *createGraphicsPipeline() const override;

        ITexture *createTexture() const override;

        IMesh *createMesh() const override;

        IShader *createShader() const override;
    };
}

