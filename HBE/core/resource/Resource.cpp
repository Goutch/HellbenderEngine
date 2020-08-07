#include "Resource.h"
#if RENDERER==OPENGL_RENDERER
#include "graphics/gl/GL_Core.h"
#elif RENDERER==VULKAN_RENDERER

#endif


template<> Texture* Resource::get<Texture>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_Texture();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}

template<> Material* Resource::get<Material>() {
    return new Material();
}

template<> ShaderProgram* Resource::get<ShaderProgram>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_ShaderProgram();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}
template<> Mesh* Resource::get<Mesh>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_Mesh();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}
template<> Framebuffer* Resource::get<Framebuffer>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_MultisamplingFramebuffer();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}