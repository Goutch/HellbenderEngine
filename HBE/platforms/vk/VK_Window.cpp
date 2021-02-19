
#include "VK_Window.h"
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
namespace HBE {
    void VK_Window::windowSizeCallback(GLFWwindow *handle, int width, int height) {
        Window *window = (Window *) glfwGetWindowUserPointer(handle);
        window->onWindowSizeChange.invoke(width, height);
    }

    VK_Window::VK_Window(int width,int height) {
        if (!glfwInit()) {
            Log::error("Failed to load glfw");
        }
        glfwSetWindowUserPointer(handle, (void *) this);
        glfwSetWindowSizeCallback(handle, windowSizeCallback);
    }

    VK_Window::~VK_Window() {
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

    void VK_Window::getSize(int &width, int &height) {
        return glfwGetWindowSize(handle,&width, &height);
    }

    GLFWwindow *VK_Window::getHandle() {
        return handle;
    }
}
