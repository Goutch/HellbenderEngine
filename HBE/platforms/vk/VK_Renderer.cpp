

#include "VK_Renderer.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "Configs.h"
GLFWwindow *HBE::VK_Renderer::createWindow(int32 width, int32 height) {
    if (!glfwInit()) {
        Log::error("Failed to load glfw");
    }
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

    GLFWwindow *window=glfwCreateWindow(width,height,Configs::getWindowTitle().c_str(), nullptr, nullptr);
    return window;
}

HBE::VK_Renderer::~VK_Renderer() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
