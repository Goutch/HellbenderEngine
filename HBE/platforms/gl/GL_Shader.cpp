
#include "glad/glad.h"
#include "GL_Shader.h"
#include "core/utility/Log.h"

namespace HBE{
    GL_Shader::~GL_Shader() {
        glDeleteShader(handle);
    }

    void GL_Shader::setSource(const std::string &source, SHADER_TYPE type) {
        unsigned int gl_shader_type;
        switch (type) {
            case HBE::SHADER_TYPE::VERTEX:

                gl_shader_type=GL_VERTEX_SHADER;
                break;
            case SHADER_TYPE::FRAGMENT:
                gl_shader_type=GL_FRAGMENT_SHADER;
                break;
            case SHADER_TYPE::COMPUTE:
                gl_shader_type=GL_COMPUTE_SHADER;
                break;
            case SHADER_TYPE::GEOMETRY:
                gl_shader_type=GL_GEOMETRY_SHADER;
                break;
            default:
                gl_shader_type=0;
        }
        handle = glCreateShader(gl_shader_type);
        const char *src = source.c_str();
        glShaderSource(handle, 1, &src, nullptr);
        glCompileShader(handle);
        int result;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
            char *message =  new char[length];
            glGetShaderInfoLog(handle, length, &length, message);
            Log::error(message);
            delete[] message;
        }
    }
}
