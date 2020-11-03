#include "Framebuffer.h"
#include <graphics/gl/GL_MultisamplingFramebuffer.h>
#include <graphics/gl/GL_Framebuffer.h>
#include "Configs.h"

Framebuffer *Framebuffer::create() {
#if RENDERER == OPENGL_RENDERER
    if (Configs::getAntialiasing()) {
        return new GL_MultisamplingFramebuffer();
    } else{
        return new GL_Framebuffer();
    }

#elif RENDERER == VULKAN_RENDERER

#endif
    return nullptr;
}
