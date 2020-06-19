//
// Created by User on 14-Jun.-2020.
//

#include <graphics/gl/GL_Framebuffer.h>
#include <graphics/gl/GL_MultisamplingFramebuffer.h>
#include <core/PrototypeEngine.h>
#include "Layer.h"
#include "glm/gtc/matrix_transform.hpp"

Layer::Layer(unsigned int width, unsigned int height, const ShaderProgram &shader_program) {
    this->width = width;
    this->heigth = height;
    this->shader_program = &shader_program;
    framebuffer = new GL_MultisamplingFramebuffer(width, height);
    projection_matrix = glm::ortho(-0.5, 0.5, -0.5, 0.5, -1.0, 1.0);
}

const glm::mat4 &Layer::getProjectionMatrix() {
    return projection_matrix;
}
void Layer::setProjectionMatrix(glm::mat4 projection_matrix){
    this->projection_matrix=projection_matrix;
}

const Framebuffer &Layer::getFramebuffer() {
    return *framebuffer;
}

const ShaderProgram &Layer::getShaderProgram() {
    return *shader_program;
}
void Layer::setShaderProgram(ShaderProgram& shader_program){
    this->shader_program=&shader_program;
}

unsigned int Layer::getWidth() {
    return width;
}

unsigned int Layer::getHeight() {
    return heigth;
}

void Layer::setSize(unsigned int width, unsigned int height) {
    this->width = width;
    this->heigth = height;
    framebuffer->setResolution(width, height);
}

void Layer::setShaderUniforms() const {
    shader_program->setUniform("texture_0",0);
    shader_program->setUniform("projection_matrix",projection_matrix);
    shader_program->setUniform("resolution",glm::vec2(width,heigth));
    shader_program->setUniform("time",PrototypeEngine::getTime());
}




