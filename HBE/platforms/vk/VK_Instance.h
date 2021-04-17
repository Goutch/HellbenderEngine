#pragma once

#include <vector>
#include "Core.h"
#include "vulkan/vulkan.h"
#include "VK_ValidationLayers.h"

namespace HBE {
    class VK_Instance {
        VkInstance handle;
        VK_ValidationLayers* validation_layers= nullptr;
    public:
        VK_Instance();

        ~VK_Instance();

    private:
        void getRequiredExtensions(std::vector<const char *> &required_extensions);
        bool checkExtensionsSupported(std::vector<const char *> &required_extensions);
    };
}