#include "glad/glad.h"
#include "GL_GraphicPipeline.h"
#include "core/utility/Log.h"

#include <glm/gtc/type_ptr.hpp>
namespace HBE {

    void GL_GraphicPipeline::setShaders(const std::vector<Shader*>& shaders) {
        for (const auto shader:shaders) {
            glAttachShader(program_id, shader->getHandle());
        }
        glLinkProgram(program_id);
        glValidateProgram(program_id);
    }

    GL_GraphicPipeline::GL_GraphicPipeline() {
        program_id = glCreateProgram();
    }

    GL_GraphicPipeline::~GL_GraphicPipeline() {
        glDeleteProgram(program_id);
    }

    void GL_GraphicPipeline::bind() const {
        glUseProgram(program_id);
    }

    void GL_GraphicPipeline::unbind() const {
        glUseProgram(0);
    }

    int GL_GraphicPipeline::uniformLocation(std::string name) const {
        auto uniform_it = uniforms.find(name);
        if (uniform_it != uniforms.end())
            return uniform_it->second;
        int location = glGetUniformLocation(program_id, name.c_str());;
        uniforms[name] = location;
        return location;
    }

    void GL_GraphicPipeline::setUniform(std::string name, int i) const {
        glUniform1i(uniformLocation(name), i);
    }

    void GL_GraphicPipeline::setUniform(std::string name, float f) const {
        glUniform1f(uniformLocation(name), f);
    }

    void GL_GraphicPipeline::setUniform(std::string name, const glm::vec2 &v) const {
        glUniform2f(uniformLocation(name), v.x, v.y);
    }

    void GL_GraphicPipeline::setUniform(std::string name, const vec3 &v) const {
        glUniform3f(uniformLocation(name), v.x, v.y, v.z);
    }

    void GL_GraphicPipeline::setUniform(std::string name, const vec4 &v) const {
        glUniform4f(uniformLocation(name), v.x, v.y, v.z, v.w);
    }

    void GL_GraphicPipeline::setUniform(std::string name, const glm::mat3 &m) const {
        glUniformMatrix3fv(uniformLocation(name), 1, false, value_ptr(m));
    }

    void GL_GraphicPipeline::setUniform(std::string name, const mat4 &m) const {
        glUniformMatrix4fv(uniformLocation(name), 1, false, value_ptr(m));
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, int i) const {
        glUniform1i(location, i);
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, float f) const {
        glUniform1f(location, f);
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, const glm::vec2 &v) const {
        glUniform2f(location, v.x, v.y);
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, const vec3 &v) const {
        glUniform3f(location, v.x, v.y, v.z);
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, const vec4 &v) const {
        glUniform4f(location, v.x, v.y, v.z, v.w);
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, const glm::mat3 &m) const {
        glUniformMatrix3fv(location, 1, false, value_ptr(m));
    }

    void GL_GraphicPipeline::setUniform(unsigned int location, const mat4 &m) const {
        glUniformMatrix4fv(location, 1, false, value_ptr(m));
    }

    void GL_GraphicPipeline::setUniformIntArray(unsigned int location, int *i, unsigned int count) const {
        glUniform1iv(location, count, i);
    }

    void GL_GraphicPipeline::setUniformFloatArray(unsigned int location, float *f, unsigned int count) const {
        glUniform1fv(location, count, f);
    }

    void GL_GraphicPipeline::setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const {
        const float *flat_array = &v[0].x;
        glUniform2fv(location, count, flat_array);
    }

    void GL_GraphicPipeline::setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const {
        const float *flat_array = &v[0].x;
        glUniform3fv(location, count, flat_array);
    }

    void GL_GraphicPipeline::setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const {
        const float *flat_array = &v[0].x;

        glUniform4fv(location, count, flat_array);
    }
}






