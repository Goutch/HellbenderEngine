#pragma once

#include "vector"
#include "vulkan/vulkan.h"


namespace HBE
{
    class VK_ValidationLayers
    {
        VkInstance instance_handle = nullptr;
        std::vector<VkValidationFeatureEnableEXT> ENABLED_VALIDATION_FEATURES{
            VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
            VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
            VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
        };
        VkDebugUtilsMessengerEXT debugMessenger {};
        VkDebugUtilsMessengerCreateInfoEXT create_info{};


    public:
        const std::vector<const char*> validation_layer_names = {
            "VK_LAYER_KHRONOS_validation"
        };

    private:
        VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks* pAllocator,
                                              VkDebugUtilsMessengerEXT* pDebugMessenger);
        void destroyDebugUtilsMessengerEXT();

    public:
        VK_ValidationLayers() = default;
        ~VK_ValidationLayers() = default;
        VK_ValidationLayers(const VK_ValidationLayers&) = delete;
        VK_ValidationLayers(VK_ValidationLayers&&) = delete;
        void init(VkInstance instance_handle);
        void release();

        bool checkValidationLayerSupport();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

        std::vector<VkValidationFeatureEnableEXT> & getEnabledValidationFeatures();
        VkDebugUtilsMessengerCreateInfoEXT& getCreateInfo();
    };
}
