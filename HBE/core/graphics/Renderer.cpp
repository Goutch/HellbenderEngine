//
// Created by User on 03-Nov.-2020.
//


#include "Renderer.h"
#include <platforms/gl/GL_Renderer.h>

namespace HBE {
    Renderer *Renderer::create() {
#if RENDERER == OPENGL_RENDERER
        return new GL_Renderer();
#elif RENDERER == VULKAN_RENDERER

#endif
    }

}