//
// Created by User on 03-Nov.-2020.
//


#include "Renderer.h"
#ifdef OPENGL_RENDERER
#include <platforms/gl/GL_Renderer.h>
#else
#ifdef VULKAN_RENDERER
#include <platforms/vk/VK_Renderer.h>
#endif
#endif

namespace HBE {
    Renderer *Renderer::create() {
#ifdef OPENGL_RENDERER
        return new GL_Renderer();
#else
#ifdef VULKAN_RENDERER
        return new VK_Renderer();
#endif
#endif
        return nullptr;
    }



}