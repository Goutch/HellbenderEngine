#pragma once
#include "vulkan/vulkan.h"
namespace HBE {
    class VK_Device;
    class VK_Fence {
        const VK_Device* device;
        VkFence handle;
    public:
        VK_Fence(const VK_Device& device);

        ~VK_Fence();
        void wait() const;
        void reset() const;
        const VkFence & getHandle() const;
    };

}

