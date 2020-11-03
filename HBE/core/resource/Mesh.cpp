
#include "Mesh.h"
#include <graphics/gl/GL_Mesh.h>
Mesh* Mesh::create() {
#if RENDERER == OPENGL_RENDERER
    return new GL_Mesh();
#elif RENDERER == VULKAN_RENDERER

#endif
    return nullptr;
}