#define GLEW_STATIC

#include "glad/glad.h"
#include "GL_ShaderProgram.h"
#include "core/utility/Log.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
void GL_ShaderProgram::setShaders(std::string vertex_path, std::string fragment_path) {

    Log::status("Compiling vertex shader:" + vertex_path);
    std::string sourcevs;
     sourcevs = getSource(vertex_path);
    unsigned int vs = compileShader(GL_VERTEX_SHADER, sourcevs);

    Log::status("Compiling fragment shader:" + fragment_path);
    std::string sourcefs;
    sourcefs = getSource(fragment_path) ;
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, sourcefs);

    glAttachShader(program_id, vs);
    glAttachShader(program_id, fs);
    glLinkProgram(program_id);
    glValidateProgram(program_id);
    Log::debug("program id=" + std::to_string(program_id));
    glDeleteShader(vs);
    glDeleteShader(fs);
}
GL_ShaderProgram::GL_ShaderProgram() {
    program_id = glCreateProgram();
}


GL_ShaderProgram::GL_ShaderProgram(const std::string &vertexShader, const std::string &fragmentShader) {
    program_id = glCreateProgram();
    setShaders(vertexShader,fragmentShader);
}

GL_ShaderProgram::GL_ShaderProgram(const std::string &vertexShader, const std::string &geometryShader, const std::string &fragmentShader,
                                   bool source) {
    program_id = glCreateProgram();

    if (!source)Log::status("Compiling vertex getShader:" + vertexShader);
    std::string sourcevs;
    !source ? sourcevs = getSource(vertexShader) : sourcevs = vertexShader;
    unsigned int vs = compileShader(GL_VERTEX_SHADER, sourcevs);

    if (!source)Log::status("Compiling geometry getShader:" + geometryShader);
    std::string sourcegs;
    !source ? sourcegs = getSource(geometryShader) : sourcegs = geometryShader;
    unsigned int gs = compileShader(GL_GEOMETRY_SHADER, sourcegs);

    if (!source)Log::status("Compiling fragment getShader:" + fragmentShader);
    std::string sourcefs;
    !source ? sourcefs = getSource(fragmentShader) : sourcefs = fragmentShader;
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, sourcefs);

    glAttachShader(program_id, vs);
    glAttachShader(program_id, gs);
    glAttachShader(program_id, fs);
    glLinkProgram(program_id);
    glValidateProgram(program_id);
    Log::debug("program id=" + std::to_string(program_id));
    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);

}

GL_ShaderProgram::~GL_ShaderProgram() {
    glDeleteProgram(program_id);
}

void GL_ShaderProgram::bind() const {
    glUseProgram(program_id);
}

void GL_ShaderProgram::unbind() const {
    glUseProgram(0);
}

int GL_ShaderProgram::uniformLocation(std::string name) const {
    if (uniforms.find(name) != uniforms.end())
        return uniforms.at(name);
    int location = glGetUniformLocation(program_id, name.c_str());;
    uniforms[name] = location;
    return location;
}

void GL_ShaderProgram::setUniform(std::string name, int i) const {
    glUniform1i(uniformLocation(name), i);
}

void GL_ShaderProgram::setUniform(std::string name, float f) const {
    glUniform1f(uniformLocation(name), f);
}

void GL_ShaderProgram::setUniform(std::string name, const glm::vec2 &v) const {
    glUniform2f(uniformLocation(name), v.x, v.y);
}

void GL_ShaderProgram::setUniform(std::string name, const vec3 &v) const {
    glUniform3f(uniformLocation(name), v.x, v.y, v.z);
}

void GL_ShaderProgram::setUniform(std::string name, const vec4 &v) const {
    glUniform4f(uniformLocation(name), v.x, v.y, v.z, v.w);
}

void GL_ShaderProgram::setUniform(std::string name, const glm::mat3 &m) const {
    glUniformMatrix3fv(uniformLocation(name), 1, false, value_ptr(m));
}

void GL_ShaderProgram::setUniform(std::string name, const mat4 &m) const {
    glUniformMatrix4fv(uniformLocation(name), 1, false, value_ptr(m));
}

void GL_ShaderProgram::setUniform(unsigned int location, int i) const {
    glUniform1i(location, i);
}

void GL_ShaderProgram::setUniform(unsigned int location, float f) const {
    glUniform1f(location, f);
}

void GL_ShaderProgram::setUniform(unsigned int location, const glm::vec2 &v) const {
    glUniform2f(location, v.x, v.y);
}

void GL_ShaderProgram::setUniform(unsigned int location, const vec3 &v) const {
    glUniform3f(location, v.x, v.y, v.z);
}

void GL_ShaderProgram::setUniform(unsigned int location, const vec4 &v) const {
    glUniform4f(location, v.x, v.y, v.z, v.w);
}

void GL_ShaderProgram::setUniform(unsigned int location, const glm::mat3 &m) const {
    glUniformMatrix3fv(location, 1, false, value_ptr(m));
}

void GL_ShaderProgram::setUniform(unsigned int location, const mat4 &m) const {
    glUniformMatrix4fv(location, 1, false, value_ptr(m));
}

void GL_ShaderProgram::setUniformIntArray(unsigned int location, int *i, unsigned int count) const {
    glUniform1iv(location, count, i);
}

void GL_ShaderProgram::setUniformFloatArray(unsigned int location, float *f, unsigned int count) const {
    glUniform1fv(location, count, f);
}

void GL_ShaderProgram::setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const {
    const float *flat_array = &v[0].x;
    glUniform2fv(location, count, flat_array);
}

void GL_ShaderProgram::setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const {
    const float *flat_array = &v[0].x;
    glUniform3fv(location, count, flat_array);
}

void GL_ShaderProgram::setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const {
    const float *flat_array = &v[0].x;

    glUniform4fv(location, count, flat_array);
}


unsigned int GL_ShaderProgram::compileShader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        Log::error(message);
        free(message);
    }
    return id;
}

std::string GL_ShaderProgram::getSource(const std::string &path) {
    try {
        std::ifstream file;
        file.open(path);
        if (file.is_open()) {
            std::stringstream strStream;
            strStream << file.rdbuf();
            std::string str = strStream.str();
            file.close();
            return str;
        } else {
            Log::error("Unable to find file:" + path);
        }

    }
    catch (std::exception &e) {
        Log::error("failed to read file " + path + "\n" + e.what());
    }
    return "";
}







