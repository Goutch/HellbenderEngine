#include "Framebuffer.h"
#include <platforms/gl/GL_MultisamplingFramebuffer.h>
#include <platforms/gl/GL_Framebuffer.h>
#include "Configs.h"

namespace HBE {
    Framebuffer *Framebuffer::create() {
#ifdef OPENGL_RENDERER
        if (Configs::getAntialiasing()) {
            return new GL_MultisamplingFramebuffer();
        } else {
            return new GL_Framebuffer();
        }

#else
#ifdef VULKAN_RENDERER
#endif
#endif
        return nullptr;
    }

}