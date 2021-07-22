#pragma once

#include <core/resource/GraphicPipeline.h>
#include <core/resource/VertexLayout.h>
#include "vulkan/vulkan.h"

namespace HBE {
    class VK_Device;

    class VK_RenderPass;

    class VK_Renderer;

    class VK_VertexLayout;

    class VK_GraphicPipeline : public GraphicPipeline {
        VK_Device *device = nullptr;
        VK_Renderer *renderer = nullptr;
        VkPipelineLayout pipeline_layout_handle{};
        VkPipeline handle = VK_NULL_HANDLE;
    public:
        VK_GraphicPipeline(VK_Device *device, VK_Renderer *renderer);

        void setDrawFlags(DRAW_FLAGS flags) override;

        DRAW_FLAGS getDrawFlags() const override;

        void setShaders(const Shader *vertex, const Shader *fragment, const VertexLayout *layout);

        void setShaders(const Shader *vertex, const Shader *geometry, const Shader *fragment, const VertexLayout *layout);

        ~VK_GraphicPipeline() override;

        void bind() const override;

        void unbind() const override;

        void setUniform(const std::string &name, int i) const override;

        void setUniform(const std::string &name, float f) const override;

        void setUniform(const std::string &name, const glm::vec2 &v) const override;

        void setUniform(const std::string &name, const glm::vec3 &v) const override;

        void setUniform(const std::string &name, const glm::vec4 &v) const override;

        void setUniform(const std::string &name, const glm::mat3 &m) const override;

        void setUniform(const std::string &name, const glm::mat4 &m) const override;

        void setUniform(unsigned int location, int i) const override;

        void setUniform(unsigned int location, float f) const override;

        void setUniform(unsigned int location, const glm::vec2 &v) const override;

        void setUniform(unsigned int location, const glm::vec3 &v) const override;

        void setUniform(unsigned int location, const glm::vec4 &v) const override;

        void setUniform(unsigned int location, const glm::mat3 &m) const override;

        void setUniform(unsigned int location, const glm::mat4 &m) const override;

        void setUniformIntArray(unsigned int location, int *i, unsigned int count) const override;

        void setUniformFloatArray(unsigned int location, float *f, unsigned int count) const override;

        void setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const override;

        void setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const override;

        void setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const override;

    };
}
