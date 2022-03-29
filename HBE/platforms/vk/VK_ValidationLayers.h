#pragma once

#include "vector"
#include "Core.h"
#include "vulkan/vulkan.h"
#include "core/utility/Log.h"


namespace HBE {
    class VK_ValidationLayers {
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT create_info;
        const VkInstance *instance_handle;
    public:
       const std::vector<const char *> validation_layer_names={
               "VK_LAYER_KHRONOS_validation"
			   ""
       };
    private:

        VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                              const VkAllocationCallbacks *pAllocator,
                                              VkDebugUtilsMessengerEXT *pDebugMessenger);
        void destroyDebugUtilsMessengerEXT();

    public:
        bool checkValidationLayerSupport();

        VK_ValidationLayers();
        void init(const VkInstance &instance_handle);
        ~VK_ValidationLayers();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

        VkDebugUtilsMessengerCreateInfoEXT& getCreateInfo();
    };
}

