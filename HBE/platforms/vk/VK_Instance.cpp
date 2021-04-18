
#include <cstring>
#include "VK_Instance.h"
#include "vector"
#include "core/utility/Log.h"
#include "Configs.h"
#include "GLFW/glfw3.h"
#include "VK_ValidationLayers.h"

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

        if (!checkExtensionsSupported(required_extensions))
            Log::error("Missing extensions to create vulkan instance");

        if (ENABLE_VALIDATION_LAYERS) {
            validation_layers = new VK_ValidationLayers();
            if (!validation_layers->checkValidationLayerSupport())
                Log::error("Vulkan validation layers not supported");
            create_info.enabledLayerCount = validation_layers->validation_layer_names.size();
            create_info.ppEnabledLayerNames = validation_layers->validation_layer_names.data();


            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &validation_layers->getCreateInfo();
        } else {
            create_info.enabledLayerCount = 0;
            create_info.pNext = nullptr;
        }

        if (vkCreateInstance(&create_info, nullptr, &handle) != VK_SUCCESS)
            Log::error("Failed to create vulkan instance!");

        if (ENABLE_VALIDATION_LAYERS)
            validation_layers->init(handle);

        Log::message("Created vulkan instance succesfully");
    }

    void VK_Instance::getRequiredExtensions(std::vector<const char *> &required_extensions) {
        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        for (unsigned int i = 0; i < glfw_extension_count; ++i) {
            required_extensions.push_back(glfw_extensions[i]);
        }
        if (ENABLE_VALIDATION_LAYERS)
            required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    bool VK_Instance::checkExtensionsSupported(std::vector<const char *> &extensions) {
        uint32_t instance_extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
        std::vector<VkExtensionProperties> instance_supported_extensions(instance_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                               instance_supported_extensions.data());


        std::string extension_support_message = "\nLooking for required vulkan extensions:";
        bool succes = true;
        for (auto extension_name:extensions) {
            extension_support_message += "\n\t";
            extension_support_message += extension_name;
            bool found = false;
            for (const auto &supported_extension:instance_supported_extensions) {
                if (strcmp(extension_name, supported_extension.extensionName) == 0) {
                    extension_support_message += "...FOUND!";
                    found = true;
                    break;
                }
            }
            if (!found) {
                succes = false;
                extension_support_message += "...MISSING!";
            }
        }
        Log::message(extension_support_message);
        return succes;
    }

    VK_Instance::~VK_Instance() {
        if (ENABLE_VALIDATION_LAYERS)
            delete validation_layers;
        vkDestroyInstance(handle, nullptr);
    }

    const VkInstance &VK_Instance::getHandle() {
        return handle;
    }
}

