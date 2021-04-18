#pragma once

#include <core/resource/GraphicPipeline.h>

namespace HBE
{
    class VK_GraphicPipeline: public GraphicPipeline {
    public:
        ~VK_GraphicPipeline() override;

        void bind() const override;

        void unbind() const override;

        void setShaders(const std::vector<Shader *> &shaders) override;

        void setUniform(std::string name, int i) const override;

        void setUniform(std::string name, float f) const override;

        void setUniform(std::string name, const glm::vec2 &v) const override;

        void setUniform(std::string name, const glm::vec3 &v) const override;

        void setUniform(std::string name, const glm::vec4 &v) const override;

        void setUniform(std::string name, const glm::mat3 &m) const override;

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

        void setUniform(std::string name, const glm::mat4 &m) const override;
    };
}
