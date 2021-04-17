
#include "GraphicPipeline.h"
#include "Shader.h"
#include <platforms/gl/GL_GraphicPipeline.h>
#include "core/serialization/Serializer.h"

namespace HBE {
    GraphicPipeline *GraphicPipeline::create() {
#ifdef OPENGL_RENDERER
        return new GL_GraphicPipeline();
#else
#ifdef VULKAN_RENDERER
        Log::error("VulkanGraphicPipeline not implemented");
#endif
#endif
        return nullptr;
    }
}