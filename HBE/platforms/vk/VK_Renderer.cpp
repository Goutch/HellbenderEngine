

#include "VK_Renderer.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "Configs.h"

void HBE::VK_Renderer::init() {
    /*VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = Configs::getWindowTitle().c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Hellbender";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        Log::error("failed to create instance!");
    }*/
}
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

void HBE::VK_Renderer::render(const HBE::RenderTarget *render_target, const mat4 &projection_matrix,
                              const mat4 &view_matrix) {

}

void HBE::VK_Renderer::present(const HBE::RenderTarget *render_target) {

}

void HBE::VK_Renderer::clearDrawCache() {

}

void HBE::VK_Renderer::draw(const HBE::Transform &transform, const HBE::Mesh &mesh, const HBE::Material &material,
                            HBE::DRAW_FLAGS flags) {

}

void HBE::VK_Renderer::drawInstanced(const HBE::Mesh &mesh, const HBE::Material &material, HBE::DRAW_FLAGS drawFlags) {

}

void HBE::VK_Renderer::clear() const {

}
