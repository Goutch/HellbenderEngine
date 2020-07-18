#pragma once

#include "glm/glm.hpp"
#include "string"

class ShaderProgram {
public:

    ShaderProgram() {};

    virtual ~ShaderProgram() {};

    virtual void bind() const = 0;

    virtual void unbind() const = 0;

    virtual void setShaders(std::string vertex_path, std::string fragment_path) = 0;

    virtual void setComputeShader(std::string compute_path) = 0;

    virtual void setUniform(std::string name, int i) const = 0;

    virtual void setUniform(std::string name, float f) const = 0;

    virtual void setUniform(std::string name, const glm::vec2 &v) const = 0;

    virtual void setUniform(std::string name, const glm::vec3 &v) const = 0;

    virtual void setUniform(std::string name, const glm::vec4 &v) const = 0;

    virtual void setUniform(std::string name, const glm::mat3 &m) const = 0;

    virtual void setUniform(unsigned int location, int i) const = 0;

    virtual void setUniform(unsigned int location, float f) const = 0;

    virtual void setUniform(unsigned int location, const glm::vec2 &v) const = 0;

    virtual void setUniform(unsigned int location, const glm::vec3 &v) const = 0;

    virtual void setUniform(unsigned int location, const glm::vec4 &v) const = 0;

    virtual void setUniform(unsigned int location, const glm::mat3 &m) const = 0;

    virtual void setUniform(unsigned int location, const glm::mat4 &m) const = 0;

    virtual void setUniformIntArray(unsigned int location, int *i, unsigned int count) const = 0;

    virtual void setUniformFloatArray(unsigned int location, float *f, unsigned int count) const = 0;

    virtual void setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const = 0;

    virtual void setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const = 0;

    virtual void setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const = 0;

    virtual void setUniform(std::string name, const glm::mat4 &m) const = 0;
};

