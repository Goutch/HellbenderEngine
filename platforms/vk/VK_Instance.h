#pragma once

#include <vector>
#include "Core.h"
#include "VK_ValidationLayers.h"
#include "vulkan/vulkan.h"

namespace HBE
{
    struct ContextInfo;

    class VK_Instance
    {
#ifdef DEBUG_MODE
        const bool ENABLE_VALIDATION_LAYERS = true;
#else
        const bool ENABLE_VALIDATION_LAYERS = false;
#endif
        bool validation_enabled = ENABLE_VALIDATION_LAYERS;
        VK_ValidationLayers validation_layers;
        VkInstance handle{};


    public:
        void init(ContextInfo& context_info);
        void release();
        VK_Instance() = default;
        ~VK_Instance() = default;
        VK_Instance(const VK_Instance&) = delete;
        VK_Instance& operator=(const VK_Instance&) = delete;

        VkInstance getHandle() const;

    private:
        void getRequiredExtensions(std::vector<const char*>& required_extensions);

        bool checkExtensionsSupported(std::vector<const char*>& required_extensions);
    };
}
