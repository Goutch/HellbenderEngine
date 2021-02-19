
#include "VK_Instance.h"
#include "vector"
#include "core/utility/Log.h"
#include "Configs.h"
#include "GLFW/glfw3.h"
#include "cstring"

namespace HBE {

    VK_Instance::VK_Instance() {
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = Configs::getWindowTitle().c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Hellbender";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        create_info.enabledExtensionCount = glfwExtensionCount;
        create_info.ppEnabledExtensionNames = glfwExtensions;

        create_info.enabledLayerCount = 0;

        if (vkCreateInstance(&create_info, nullptr, &handle) != VK_SUCCESS) {
            Log::error("Failed to create vulkan instance!");
        }
        Log::message("Created vulkan instance succesfully");

        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

        Log::message("available extensions:");

        for (const auto &extension : extensions) {
            Log::message('\t' + extension.extensionName);
        }
    }

    VK_Instance::~VK_Instance() {
        vkDestroyInstance(handle, nullptr);
    }
}

