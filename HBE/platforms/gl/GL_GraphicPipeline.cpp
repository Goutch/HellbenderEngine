#include "glad/glad.h"
#include "GL_GraphicPipeline.h"
#include "core/utility/Log.h"
#include "GL_Shader.h"
#include <glm/gtc/type_ptr.hpp>

namespace HBE {


    void GL_GraphicPipeline::setDrawFlags(DRAW_FLAGS flags) {

    }

    DRAW_FLAGS GL_GraphicPipeline::getDrawFlags() const {
        return 0;
    }

    void GL_GraphicPipeline::setShaders(const Shader *vertex, const Shader *fragment) {
        unsigned int vert = *(unsigned int *) (vertex->getHandle());
        unsigned int frag = *(unsigned int *) (fragment->getHandle());
        glAttachShader(program_id, vert);
        glAttachShader(program_id, frag);
        glLinkProgram(program_id);
        glValidateProgram(program_id);
        int result;
        glGetProgramiv(program_id, GL_LINK_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &length);
            char *message = new char[length];
            glGetShaderInfoLog(program_id, length, &length, message);
            Log::error(message);
            delete[] message;
        }
    }

    void GL_GraphicPipeline::setShaders(const Shader *vertex, const Shader *geometry, const Shader *fragment) {
        glAttachShader(program_id, *(unsigned int *) (vertex->getHandle()));
        glAttachShader(program_id, *(unsigned int *) (geometry->getHandle()));
        glAttachShader(program_id, *(unsigned int *) (fragment->getHandle()));
        glLinkProgram(program_id);
        glValidateProgram(program_id);
        /*int result;
        glGetShaderiv(program_id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &length);
            char *message = new char[length];
            glGetShaderInfoLog(program_id, length, &length, message);
            Log::error(message);
            delete[] message;
        }*/
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

    void GL_GraphicPipeline::setUniform(const std::string &name, int i) const {
        glUniform1i(uniformLocation(name), i);
    }

    void GL_GraphicPipeline::setUniform(const std::string &name, float f) const {
        glUniform1f(uniformLocation(name), f);
    }

    void GL_GraphicPipeline::setUniform(const std::string &name, const glm::vec2 &v) const {
        glUniform2f(uniformLocation(name), v.x, v.y);
    }

    void GL_GraphicPipeline::setUniform(const std::string &name, const vec3 &v) const {
        glUniform3f(uniformLocation(name), v.x, v.y, v.z);
    }

    void GL_GraphicPipeline::setUniform(const std::string &name, const vec4 &v) const {
        glUniform4f(uniformLocation(name), v.x, v.y, v.z, v.w);
    }

    void GL_GraphicPipeline::setUniform(const std::string &name, const glm::mat3 &m) const {
        glUniformMatrix3fv(uniformLocation(name), 1, false, value_ptr(m));
    }

    void GL_GraphicPipeline::setUniform(const std::string &name, const mat4 &m) const {
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






