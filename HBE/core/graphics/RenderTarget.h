#pragma once
#include "glm/glm.hpp"
#include "core/utility/Event.h"
#include "Core.h"

using namespace glm;
namespace HBE {
    class Framebuffer;

    class GraphicPipeline;

    class HB_API RenderTarget {
        unsigned int width;
        unsigned int heigth;
        mat4 projection_matrix;
        Framebuffer *framebuffer;
    public:
        Event<int, int> onSizeChange;

        RenderTarget(unsigned int width, unsigned int height, const GraphicPipeline &shader_program);

        const GraphicPipeline *shader_program;

        const Framebuffer &getFramebuffer() const;

        const GraphicPipeline &getShaderProgram() const;

        const mat4 &getProjectionMatrix();

        unsigned int getWidth() const;

        unsigned int getHeight() const;

        void setSize(unsigned int width, unsigned int height);

        void setShaderUniforms() const;

        void setProjectionMatrix(mat4 projection_matrix);

        void setShaderProgram(GraphicPipeline &shader_program);

        unsigned int getTextureID();
    };
}
