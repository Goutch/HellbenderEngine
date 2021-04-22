//
// Created by User on 14-Jun.-2020.
//
#include "RenderTarget.h"
#include <core/serialization/Serializable.h>
#include "glm/gtc/matrix_transform.hpp"
#include "Framebuffer.h"
#include "core/resource/GraphicPipeline.h"
namespace HBE {
    RenderTarget::RenderTarget(unsigned int width, unsigned int height, const GraphicPipeline &shader_program) {
        this->shader_program = &shader_program;
        framebuffer = Framebuffer::create();
        setSize(width, height);
        projection_matrix = glm::ortho(-0.5, 0.5, -0.5, 0.5, -1.0, 1.0);
    }

    const glm::mat4 &RenderTarget::getProjectionMatrix() {
        return projection_matrix;
    }

    void RenderTarget::setProjectionMatrix(glm::mat4 projection_matrix) {
        this->projection_matrix = projection_matrix;
    }

    const Framebuffer &RenderTarget::getFramebuffer() const {
        return *framebuffer;
    }

    const GraphicPipeline &RenderTarget::getShaderProgram() const {
        return *shader_program;
    }

    void RenderTarget::setShaderProgram(GraphicPipeline &shader_program) {
        this->shader_program = &shader_program;
    }

    unsigned int RenderTarget::getWidth() const {
        return width;
    }

    unsigned int RenderTarget::getHeight() const {
        return heigth;
    }

    void RenderTarget::setSize(unsigned int width, unsigned int height) {
        this->width = width;
        this->heigth = height;

        framebuffer->setResolution(width, height);

        onSizeChange.invoke(width, height);
    }

    void RenderTarget::setShaderUniforms() const {
        shader_program->setUniform("texture_0", 0);
        shader_program->setUniform("projection_matrix", projection_matrix);
        shader_program->setUniform("resolution", glm::vec2(width, heigth));
    }
}




