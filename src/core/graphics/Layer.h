#pragma once

#include <core/ressource/Framebuffer.h>
#include <core/ressource/ShaderProgram.h>

class Layer {
    unsigned int width;
    unsigned int heigth;
    glm::mat4 projection_matrix;
public:
    Layer(unsigned int width, unsigned int height, const ShaderProgram &shader_program);

    Framebuffer *framebuffer;
    const ShaderProgram *shader_program;

    const Framebuffer &getFramebuffer();

    const ShaderProgram &getShaderProgram();

    const glm::mat4 &getProjectionMatrix();

    unsigned int getWidth();

    unsigned int getHeight();

    void setSize(unsigned int width, unsigned int height);

    void setShaderUniforms() const;

    void setProjectionMatrix(glm::mat4 projection_matrix);

    void setShaderProgram(ShaderProgram &shader_program);
};

