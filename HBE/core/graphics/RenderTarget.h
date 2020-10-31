#pragma once

class Framebuffer;

class ShaderProgram;

#include "glm/glm.hpp"
#include "core/utility/Event.h"
using namespace glm;

class RenderTarget {
    unsigned int width;
    unsigned int heigth;
    mat4 projection_matrix;
    Framebuffer *framebuffer;
public:
    RenderTarget(unsigned int width, unsigned int height, const ShaderProgram &shader_program);

    const ShaderProgram *shader_program;

    const Framebuffer &getFramebuffer();

    const ShaderProgram &getShaderProgram();

    const mat4 &getProjectionMatrix();

    unsigned int getWidth() const;

    unsigned int getHeight() const;

    void setSize(unsigned int width, unsigned int height);

    void setShaderUniforms() const;

    void setProjectionMatrix(mat4 projection_matrix);

    void setShaderProgram(ShaderProgram &shader_program);
};

