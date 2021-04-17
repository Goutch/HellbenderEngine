#pragma once
#include <platforms/gl/GL_GraphicPipeline.h>
#include "GraphicPipeline.h"

namespace HBE {
    class HB_API ComputeShader : public GraphicPipeline {
    private:
        ComputeShader() {}

    public:
        virtual void dispatch(int x, int y, int z);

        static ComputeShader *create(std::string &source) {
            Shader *compute = Shader::create(SHADER_TYPE::COMPUTE, source);
#if RENDERER == OPENGL_RENDERER
            GraphicPipeline *graphicPipeline = new GL_GraphicPipeline();
            graphicPipeline->setShaders({compute});
            delete compute;
            return dynamic_cast<ComputeShader *>(graphicPipeline);
#elif RENDERER == VULKAN_RENDERER
            Log::error("Colmpute shaders are not implemented in vulkan")
            return nullptr;
#endif
        }
    };
}

