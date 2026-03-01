#pragma once

#include "vulkan/vulkan.h"
#include "dependencies/utils-collection/Event.h"
#include "core/graphics/Fence.h"

namespace HBE
{
    class VK_Context;

    class VK_Fence : public Fence
    {
        VK_Context* context = nullptr;
        VkFence handle = VK_NULL_HANDLE;

    public:
        void init(VK_Context* context);
        void release();
        VK_Fence() = default;
        VK_Fence(VK_Fence&& other) noexcept;
        VK_Fence(VK_Fence& other) = delete;
        VK_Fence& operator=(VK_Fence& other) = delete;

        ~VK_Fence() = default;
        void wait() const override;
        void reset() const;
        bool isSet() const;
        const VkFence& getHandle() const;
    };
}
