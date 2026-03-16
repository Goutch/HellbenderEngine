#include "VK_CommandPool.h"

#include "VK_Context.h"
#include "core/utility/Log.h"
#include "VK_Device.h"
#include "VK_Swapchain.h"
#include "dependencies/utils-collection/Profiler.h"
#include "VK_Queue.h"

namespace HBE
{
    void VK_CommandPool::init(VK_Context* context, uint32_t command_buffers_count, const VK_Queue& queue)
    {
        this->context = context;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue.getFamilyIndex();

        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;


        if (vkCreateCommandPool(context->device.getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS)
        {
            Log::error("failed to create command pool!");
        }
        createCommandBuffers(command_buffers_count);
    }

    void VK_CommandPool::release()
    {
        for (FenceHandle fence : fences)
        {
            context->waitForFence(fence);
            context->releaseFence(fence);
        }
        fences.clear();
        vkDestroyCommandPool(context->device.getHandle(), handle, nullptr);
    }

    void HBE::VK_CommandPool::clear()
    {
        for (FenceHandle fence : fences)
        {
            context->waitForFence(fence);
            context->releaseFence(fence);
        }
        fences.clear();

        vkFreeCommandBuffers(context->device.getHandle(), handle, static_cast<uint32_t>(command_buffers.size()),
                             command_buffers.data());
        command_buffers.clear();
    }


    void HBE::VK_CommandPool::createCommandBuffers(int count)
    {
        current = count - 1;
        for (int i = 0; i < count; ++i)
        {
            fences.emplace_back();
            context->createFence(fences[i], {});
        }
        command_buffers.resize(count);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = handle;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

        if (vkAllocateCommandBuffers(context->device.getHandle(), &allocInfo, command_buffers.data()) != VK_SUCCESS)
        {
            Log::error("failed to allocate command buffers!");
        }
    }


    void HBE::VK_CommandPool::begin() const
    {
        context->fences[fences[current]].wait();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(command_buffers[current], &beginInfo) != VK_SUCCESS)
        {
            Log::error("failed to begin recording command buffer!");
        }
    }

    void HBE::VK_CommandPool::end() const
    {
        if (vkEndCommandBuffer(command_buffers[current]) != VK_SUCCESS)
        {
            Log::error("failed to record command buffer!");
        }
    }

    FenceHandle VK_CommandPool::getCurrentFence()
    {
        return fences[current];
    }

    const std::vector<VkCommandBuffer>& HBE::VK_CommandPool::getBuffers() const
    {
        return command_buffers;
    }


    void VK_CommandPool::reset()
    {
        vkResetCommandBuffer(command_buffers[current], 0);
    }

    const VkCommandBuffer& VK_CommandPool::getCurrentBuffer() const
    {
        return command_buffers[current];
    }

    void VK_CommandPool::waitAll()
    {
        for (FenceHandle fence : fences)
        {
            context->fences[fence].wait();
            context->fences[fence].reset();
        }
    }

    VkCommandPool VK_CommandPool::getHandle() const
    {
        return handle;
    }

    FenceHandle VK_CommandPool::submit(VK_Queue& queue, VkSemaphore* wait, VkPipelineStageFlags* wait_stage,
                                       uint32_t wait_count, VkSemaphore* signal, uint32_t signal_count)
    {
        HB_PROFILE_BEGIN("resetFence");
        context->fences[fences[current]].reset();
        HB_PROFILE_END("resetFence");
        HB_PROFILE_BEGIN("submit");
        queue.submit(command_buffers[current],
                     context->fences[fences[current]].getHandle(),
                     wait,
                     wait_stage,
                     wait_count,
                     signal,
                     signal_count);
        HB_PROFILE_END("submit");
        last_summited = current;
        current++;
        current %= command_buffers.size();

        return fences[last_summited];
    }

    FenceHandle VK_CommandPool::getLastFence()
    {
        return fences[last_summited];
    }

    uint32_t VK_CommandPool::getCommandBufferIndex() const
    {
        return current;
    }
}
