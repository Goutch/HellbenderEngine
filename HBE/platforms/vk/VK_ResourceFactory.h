#pragma once

#include <core/resource/Material.h>
#include <core/resource/Model.h>
#include "core/resource/ResourceFactory.h"

namespace HBE {

    class VK_Device;

    class VK_Renderer;

    class VK_ResourceFactory : public ResourceFactory {
        const VK_Device *device;
        const VK_Renderer *renderer;
    public:
        VK_ResourceFactory(const VK_Device *device, const VK_Renderer *renderer);

        GraphicPipeline *createGraphicsPipeline() const override;

        Texture *createTexture() const override;

        Mesh *createMesh() const override;

        Shader *createShader() const override;

        ComputePipeline *createComputePipeline() const override;

        VertexLayout *createVertexLayout() const;
    };
}

