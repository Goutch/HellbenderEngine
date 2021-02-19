#include "Window.h"
#ifdef OPENGL_RENDERER
#include "platforms/gl/GL_Window.h"
#else
#ifdef VULKAN_RENDERER
#include "platforms/vk/VK_Window.h"
#endif
#endif
namespace HBE {
    Window *Window::create(int width,int height) {
#ifdef OPENGL_RENDERER
        return new GL_Window(width,height);
#else
#ifdef VULKAN_RENDERER
        return new VK_Window(width,height);
#endif
#endif
        return nullptr;
    }
}