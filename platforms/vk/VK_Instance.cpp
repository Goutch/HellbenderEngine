#include <cstring>
#include "VK_Instance.h"
#include "vector"
#include "core/utility/Log.h"
#include "GLFW/glfw3.h"
#include "VK_ValidationLayers.h"
#include "core/Application.h"

namespace HBE
{
    void VK_Instance::init(ContextInfo& context_info)
    {
        const ApplicationInfo& app_info = Application::instance->getInfo();
        VkApplicationInfo vk_app_info{};
        vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vk_app_info.pApplicationName = app_info.app_name.c_str();
        vk_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        vk_app_info.pEngineName = "Hellbender";
        vk_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        vk_app_info.apiVersion = VK_API_VERSION_1_1;
        if (context_info.vulkan_version == VULKAN_VERSION_1_2)
            vk_app_info.apiVersion = VK_API_VERSION_1_2;
        else if (context_info.vulkan_version == VULKAN_VERSION_1_3)
            vk_app_info.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &vk_app_info;
        std::vector<const char*> required_extensions;

        getRequiredExtensions(required_extensions);

        std::vector<const char*> extensions{};


        Log::message("Looking for required vulkan extensions:");
        if (!checkExtensionsSupported(required_extensions))
        {
            Log::error("Missing required extensions to create vulkan instance");
        }
        extensions.insert(extensions.end(), required_extensions.begin(), required_extensions.end());


        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();


        VkValidationFeaturesEXT validationFeaturesExt{};
        if (validation_enabled)
        {
            if (!validation_layers.checkValidationLayerSupport())
            {
                Log::warning("Vulkan validation layers not supported");
                validation_enabled = false;
            }
        }
        if (validation_enabled)
        {
            create_info.enabledLayerCount = validation_layers.validation_layer_names.size();
            create_info.ppEnabledLayerNames = validation_layers.validation_layer_names.data();


            create_info.pNext = &validationFeaturesExt;
            validationFeaturesExt.enabledValidationFeatureCount = validation_layers.getEnabledValidationFeatures().size();
            validationFeaturesExt.pEnabledValidationFeatures = validation_layers.getEnabledValidationFeatures().data();
            validationFeaturesExt.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            validationFeaturesExt.pNext = &validation_layers.getCreateInfo();
        }
        else
        {
            create_info.enabledLayerCount = 0;
            create_info.pNext = nullptr;
        }

        if (vkCreateInstance(&create_info, nullptr, &handle) != VK_SUCCESS)
            Log::error("Failed to create vulkan instance!");
        if (validation_enabled)
            validation_layers.init(handle);
        Log::status("Created vulkan instance succesfully");
    }

    void VK_Instance::release()
    {
        validation_layers.release();
        vkDestroyInstance(handle, nullptr);
    }

    void VK_Instance::getRequiredExtensions(std::vector<const char*>& required_extensions)
    {
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        for (unsigned int i = 0; i < glfw_extension_count; ++i)
        {
            required_extensions.push_back(glfw_extensions[i]);
        }
        if (ENABLE_VALIDATION_LAYERS)
        {
            required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    }

    bool VK_Instance::checkExtensionsSupported(std::vector<const char*>& extensions)
    {
        uint32_t instance_extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
        std::vector<VkExtensionProperties> instance_supported_extensions(instance_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                               instance_supported_extensions.data());


        std::string extension_support_message = "";
        bool succes = true;
        for (auto extension_name : extensions)
        {
            extension_support_message += "\t";
            extension_support_message += extension_name;

            bool found = false;
            for (const auto& supported_extension : instance_supported_extensions)
            {
                if (strcmp(extension_name, supported_extension.extensionName) == 0)
                {
                    extension_support_message += "...FOUND!";
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                succes = false;
                extension_support_message += "...MISSING!";
            }
            extension_support_message += "\n";
        }
        Log::message(extension_support_message);
        return succes;
    }

    VkInstance VK_Instance::getHandle() const
    {
        return handle;
    }
}
