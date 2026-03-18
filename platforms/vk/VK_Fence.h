#pragma once

#include "vulkan/vulkan.h"
#include "dependencies/utils-collection/Event.h"
#include "core/graphics/Fence.h"
#include "core/interface/FenceInterface.h"

namespace HBE
{
    class VK_Context;

    class VK_Fence
    {
        VK_Context* context = nullptr;
        VkFence handle = VK_NULL_HANDLE;

    public:
        void alloc(VK_Context* context,const FenceInfo& info);
        void release();

        FENCE_STATUS getStatus();

        void getStatus(FENCE_STATUS& status);
        VK_Fence() = default;
        VK_Fence(VK_Fence&& other) noexcept;
        VK_Fence(VK_Fence& other) = delete;
        VK_Fence& operator=(VK_Fence& other) = delete;
		bool allocated();
        ~VK_Fence() = default;
        void wait() const;
        void reset() const;
        bool isSet() const;
        const VkFence& getHandle() const;
    };
}
