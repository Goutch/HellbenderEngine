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
    	VK_VertexLayout(const VertexLayoutInfo& info);

        int getBytesPerVertex() const override;

        const VkVertexInputBindingDescription &getBindingDescription() const;

        const std::vector<VkVertexInputAttributeDescription> &getAttributeDescriptions() const;
    };
}


