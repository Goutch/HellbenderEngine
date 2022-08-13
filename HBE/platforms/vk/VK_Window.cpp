
#include "VK_Window.h"
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "Configs.h"
#include "core/graphics/Graphics.h"
#include "VK_RenderPass.h"

namespace HBE {
	void VK_Window::windowSizeCallback(GLFWwindow *handle, int width, int height) {
		Window *window = (Window *) glfwGetWindowUserPointer(handle);
		VK_Window *instance = reinterpret_cast<VK_Window *>(glfwGetWindowUserPointer(handle));
		Log::status("Window size changed from (" + std::to_string(instance->last_width) + " x " + std::to_string(instance->last_height) + ") to (" + std::to_string(width) + " x " + std::to_string(height) + ")");
		instance->width = width;
		instance->height = height;
		window->onSizeChange.invoke(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

	}

	void VK_Window::onTitleChange(std::string title) {
		glfwSetWindowTitle(handle, title.c_str());
	}

	VK_Window::VK_Window(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
		if (!glfwInit()) {
			Log::error("Failed to load glfw");
		}
		if (!glfwVulkanSupported()) {
			Log::error("Vulkan is not supported");
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		handle = glfwCreateWindow(width, height, Configs::getWindowTitle().c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(handle, (void *) this);
		glfwSetWindowSizeCallback(handle, windowSizeCallback);
		Configs::onWindowTitleChange.subscribe(this, &VK_Window::onTitleChange);
	}

	VK_Window::~VK_Window() {
		Configs::onWindowTitleChange.unsubscribe(this);
		glfwTerminate();
	}

	bool VK_Window::shouldClose() {
		return glfwWindowShouldClose(handle);
	}

	void VK_Window::swapBuffers() {

	}

	void VK_Window::requestClose() {
		glfwSetWindowShouldClose(handle, true);
	}

	void VK_Window::getSize(uint32_t &width, uint32_t &height) {
		width = this->width;
		height = this->height;

	}

	GLFWwindow *VK_Window::getHandle() {
		return handle;
	}

	void VK_Window::setFullscreen(bool fullscreen) {
		if (fullscreen) {
			last_width = width;
			last_height = height;
			// backup window position and window size
			glfwGetWindowPos(handle, &last_x, &last_y);

			// get resolution of monitor
			const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			// switch to full screen
			glfwSetWindowMonitor(handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			width = mode->width;
			height = mode->height;
		} else {
			// restore last window size and position
			int temp_width = width;
			int temp_height = height;
			width = last_width;
			height = last_height;
			last_width = temp_width;
			last_height = temp_height;

			glfwSetWindowMonitor(handle, nullptr, last_x, last_y, width, height, GLFW_DONT_CARE);
		}

	}

}
