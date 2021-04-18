
#include "Mesh.h"
#include "core/utility/Log.h"
#ifdef OPENGL_RENDERER
#include <platforms/gl/GL_Mesh.h>
#else
#ifdef VULKAN_RENDERER

#endif
#endif
#include "core/serialization/Serializer.h"

namespace HBE {
    Mesh *Mesh::create() {
#ifdef OPENGL_RENDERER
        return new GL_Mesh();
#else
#ifdef VULKAN_RENDERER
        Log::error("Mesh not implemented in Vulkan");
#endif
#endif
        return nullptr;
    }
}