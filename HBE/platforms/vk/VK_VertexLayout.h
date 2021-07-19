#pragma once

#include "core/resource/VertexLayout.h"
#include "vulkan/vulkan.h"

namespace HBE {
    class VK_VertexLayout : public VertexLayout {
        std::vector<GLSL_TYPE> layout;
    private:
        VkVertexInputBindingDescription binding_description{};
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
    public:
        int getBytesPerVertex() const override;

        void setLayoutTypes(std::vector<GLSL_TYPE> layout);

        const VkVertexInputBindingDescription &getBindingDescription() const;

        const std::vector<VkVertexInputAttributeDescription> &getAttributeDescriptions() const;
    };
}


