#pragma once

#include <core/resource/Material.h>
#include <core/resource/Model.h>
#include "core/resource/IResourceFactory.h"

namespace HBE {

    class VK_Device;

    class VK_ResourceFactory : public IResourceFactory {
        const VK_Device *device;
    public:

        VK_ResourceFactory(const VK_Device *device);

        IGraphicPipeline *createGraphicsPipeline() const override;

        ITexture *createTexture() const override;

        IMesh *createMesh() const override;

        IShader *createShader() const override;

        IComputePipeline *createComputePipeline() const override;
    };
}

