#include "Window.h"

#ifdef VULKAN_RENDERER

#include "platforms/vk/VK_Window.h"

#endif
namespace HBE {
	Window *Window::create(const char *title, uint32_t width, uint32_t height) {
#ifdef VULKAN_RENDERER
		return new VK_Window(title, width, height);
#endif
		return nullptr;
	}

	bool Window::isFullscreen() {
		return is_fullscreen;
	}
}