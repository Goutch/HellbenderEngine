#pragma once

#include <string>
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>
#include <core/resource/GraphicPipeline.h>

using namespace glm;
namespace HBE {
    class GL_GraphicPipeline : public GraphicPipeline {
    private:
        mutable std::unordered_map<std::string, int> uniforms;

        unsigned int program_id;

    public:
        GL_GraphicPipeline();

        ~GL_GraphicPipeline();

        void bind() const override;

        void setShaders(const std::vector<Shader*>& shaders) override;

        void unbind() const override;

        int uniformLocation(std::string) const;

        void setUniform(std::string name, int i) const;

        void setUniform(std::string name, float f) const;

        void setUniform(std::string name, const glm::vec2 &v) const;

        void setUniform(std::string name, const glm::vec3 &v) const;

        void setUniform(std::string name, const glm::vec4 &v) const;

        void setUniform(std::string name, const glm::mat3 &m) const;

        void setUniform(std::string name, const glm::mat4 &m) const;

        void setUniform(unsigned int location, int i) const;

        void setUniform(unsigned int location, float f) const;

        void setUniform(unsigned int location, const glm::vec2 &v) const;

        void setUniform(unsigned int location, const glm::vec3 &v) const;

        void setUniform(unsigned int location, const glm::vec4 &v) const;

        void setUniform(unsigned int location, const glm::mat3 &m) const;

        void setUniform(unsigned int location, const glm::mat4 &m) const;

        void setUniformIntArray(unsigned int location, int *i, unsigned int count) const;

        void setUniformFloatArray(unsigned int location, float *f, unsigned int count) const;

        void setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const;

        void setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const;

        void setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const;

    };

}