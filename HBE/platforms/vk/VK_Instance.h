#pragma once

#include <vector>
#include "Core.h"
#include "vulkan/vulkan.hpp"

namespace HBE {
    class VK_Instance {
        uint32_t extension_count;
        VkInstance handle;

    public:
        VK_Instance();

        ~VK_Instance();

    };
}