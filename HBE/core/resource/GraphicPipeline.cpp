
#include "GraphicPipeline.h"
#include "Shader.h"
#include <platforms/gl/GL_GraphicPipeline.h>
#include "core/serialization/Serializer.h"
namespace HBE {
    GraphicPipeline *GraphicPipeline::create() {
#if RENDERER == OPENGL_RENDERER
        return new GL_GraphicPipeline();
#elif RENDERER == VULKAN_RENDERER

#endif
        return nullptr;
    }
}