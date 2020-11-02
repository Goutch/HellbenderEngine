#include "Resource.h"
#if RENDERER==OPENGL_RENDERER
#include "graphics/gl/GL_Core.h"
#elif RENDERER==VULKAN_RENDERER

#endif


template<> Texture* Resource::create<Texture>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_Texture();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}


template<> Material* Resource::create<Material>() {
    return new Material();
}

template<> ShaderProgram* Resource::create<ShaderProgram>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_ShaderProgram();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}

template<> Mesh* Resource::create<Mesh>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_Mesh();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}
template<> Model* Resource::load<Model>(std::string path) {
    Model* model=new Model();
    model->load(path);
    return model;
}
template<> Model* Resource::create<Model>() {
    return new Model();
}
template<> Framebuffer* Resource::create<Framebuffer>() {
#if RENDERER==OPENGL_RENDERER
    return new GL_MultisamplingFramebuffer();
#elif RENDERER==VULKAN_RENDERER

#endif
    return nullptr;
}