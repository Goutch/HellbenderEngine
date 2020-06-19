#pragma once

#include <string>
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>
#include <core/ressource/ShaderProgram.h>

using namespace glm;

class GL_ShaderProgram : public ShaderProgram {
private:
    unsigned int compileShader(unsigned int type, const std::string &source);

    std::string getSource(const std::string &path);

    mutable std::unordered_map<std::string, int> uniforms;

    unsigned int program_id;

public:

    GL_ShaderProgram(const std::string &vertexShader, const std::string &fragmentShader, bool source = false);

    GL_ShaderProgram(const std::string &vertexShader,
                     const std::string &geometryShader,
                     const std::string &fragmentShader,
                     bool source = false);

    ~GL_ShaderProgram();

    void bind() const override;

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

