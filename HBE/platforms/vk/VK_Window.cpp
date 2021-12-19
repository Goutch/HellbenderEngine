
#include "VK_Window.h"
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "Configs.h"

namespace HBE {
	void VK_Window::windowSizeCallback(GLFWwindow *handle, int width, int height) {
		Window *window = (Window *) glfwGetWindowUserPointer(handle);
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
		int w, h;
		glfwGetWindowSize(handle, &w, &h);
		while (w == 0 || h == 0) {
			glfwGetFramebufferSize(handle, &w, &h);
			glfwWaitEvents();
		}
		width = static_cast<uint32_t>(w);
		height = static_cast<uint32_t>(h);
	}

	GLFWwindow *VK_Window::getHandle() {
		return handle;
	}

}
