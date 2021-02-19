#pragma once

#include <vector>
#include "Core.h"
#include "vulkan/vulkan.h"

namespace HBE {
    class VK_Instance {
        uint32_t extension_count;
        VkInstance handle;

    public:
        VK_Instance();

        ~VK_Instance();

    private:
        void getRequiredExtensions(std::vector<const char *> &required_extensions);
        bool checkExtensionsSupported(std::vector<const char *> &required_extensions);
    };
}