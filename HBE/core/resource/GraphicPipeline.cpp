
#include "GraphicPipeline.h"

#ifdef OPENGL_RENDERER
#include <platforms/gl/GL_GraphicPipeline.h>
#endif

#include "core/utility/Log.h"
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