#pragma once

#include <core/resource/GraphicPipeline.h>
#include <array>
#include "vulkan/vulkan.h"
#include "unordered_map"
#include "VK_Image.h"
#include "VK_CONSTANTS.h"
#include "VK_Shader.h"
#include "unordered_map"

namespace HBE {
    class VK_Device;

    class VK_RenderPass;

    class VK_Renderer;

    class VK_Buffer;

    class VK_PipelineLayout;

    class VK_GraphicPipeline : public GraphicPipeline {
        VkPipeline handle = VK_NULL_HANDLE;
        VK_PipelineLayout *layout = nullptr;

        VK_Device *device = nullptr;
        VK_Renderer *renderer = nullptr;

        std::vector<const VK_Shader *> shaders;
    public:
        VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer, const GraphicPipelineInfo &info);

        VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer, const GraphicPipelineInfo &info,
                           const VkRenderPass &render_pass);

        void setDynamicUniform(const std::string &name, const void *data) override;

        void setDynamicUniform(uint32_t binding, const void *data) override;

        void setUniform(const std::string &name, const void *data) override;

        void setUniform(uint32_t binding, const void *data) override;

        void pushConstant(const std::string &name, const void *data) override;

        void setTexture(uint32_t binding, const Texture *texture,uint32_t mip_level) override;

        void setTexture(const std::string &name, const Texture *texture,uint32_t mip_level) override;

        void setTexture(uint32_t binding, const RenderTarget *render_target) override;

        void setTexture(const std::string &name, const RenderTarget *render_target) override;

        ~VK_GraphicPipeline() override;

        void bind() const override;

        void unbind() const override;

        void createPipelineLayout();
    };
}
