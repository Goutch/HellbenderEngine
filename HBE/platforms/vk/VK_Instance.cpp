
#include <cstring>
#include "VK_Instance.h"
#include "vector"
#include "core/utility/Log.h"
#include "Configs.h"
#include "GLFW/glfw3.h"


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

        std::vector<const char *> required_extensions;
        getRequiredExtensions(required_extensions);
        create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
        create_info.ppEnabledExtensionNames = required_extensions.data();
        create_info.enabledLayerCount = 0;

        if (checkExtensionsSupported(required_extensions)) {
#ifdef DEBUG_MODE

#endif
            if (vkCreateInstance(&create_info, nullptr, &handle) != VK_SUCCESS) {
                Log::error("Failed to create vulkan instance!");
            }
        } else {
            Log::error("Missing extensions to create vulkan instance");
        }
        Log::message("Created vulkan instance succesfully");


    }

    void VK_Instance::getRequiredExtensions(std::vector<const char *> &required_extensions) {
        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        for (unsigned int i = 0; i < glfw_extension_count; ++i) {
            required_extensions.push_back(glfw_extensions[i]);
        }
#ifdef DEBUG_MODE
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    }

    bool VK_Instance::checkExtensionsSupported(std::vector<const char *> &extensions) {
        uint32_t instance_extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
        std::vector<VkExtensionProperties> instance_supported_extensions(instance_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                               instance_supported_extensions.data());


        std::string extension_support_message="\nLooking for required vulkan extensions:";
        bool succes = true;
        for (auto extension_name:extensions) {
            extension_support_message += "\n\t";
            extension_support_message += extension_name;
            bool found = false;
            for (const auto &supported_extension:instance_supported_extensions) {
                if (strcmp(extension_name, supported_extension.extensionName) == 0) {
                    extension_support_message +="...FOUND!";
                    found = true;
                    break;
                }
            }
            if (!found) {
                succes = false;
                extension_support_message +="...MISSING!";
            }
        }
        Log::message(extension_support_message);
        return succes;
    }

    VK_Instance::~VK_Instance() {
        vkDestroyInstance(handle, nullptr);
    }
}

