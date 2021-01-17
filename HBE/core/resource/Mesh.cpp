
#include "Mesh.h"
#if RENDERER == OPENGL_RENDERER
#include <platforms/gl/GL_Mesh.h>
#elif RENDERER == VULKAN_RENDERER

#endif

#include "core/serialization/Serializer.h"

namespace HBE {
    Mesh *Mesh::create() {
#if RENDERER == OPENGL_RENDERER
        return new GL_Mesh();
#elif RENDERER == VULKAN_RENDERER

#endif
        return nullptr;
    }
}