#include "VK_Buffer.h"
#include "VK_Device.h"
#include "VK_Context.h"
#include "VK_Utils.h"

namespace HBE
{
    VkBuffer VK_Buffer::getHandle() const
    {
        return handle;
    }

    void VK_Buffer::alloc(VK_Context* context, BufferInfo& info)
    {
        this->device = &context->device;

        if (context->allocator.valid(info.optional_allocator))
        {
            Log::error("allocators are not yet implemented");
        }
        else
        {
            this->allocator = &context->allocator;
        }

        if (info.size <= 0)
            return;
        this->size = info.size;
        std::vector<uint32_t> queues = {device->getQueue(QUEUE_FAMILY_GRAPHICS).getFamilyIndex()};

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = info.memory_type_flags & MEMORY_TYPE_FLAG_MAPPABLE ? info.usage : info.usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = queues.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = queues.size();
        bufferInfo.pQueueFamilyIndices = queues.data();

        if (vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &handle) != VK_SUCCESS)
        {
            Log::error("failed to create buffer!");
        }
#ifdef DEBUG_MODE
        Log::debug("Created buffer " + VK_Utils::handleToString(handle));
#endif
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(device->getHandle(), handle, &requirements);
        requirements.alignment = std::max(requirements.alignment, info.optional_custom_alignment);

        this->allocation = allocator->alloc(requirements, info.memory_type_flags);
        vkBindBufferMemory(device->getHandle(), handle, allocation.block->memory, allocation.offset);

        if (info.data != nullptr)
        {
            update(info.data);
        }
    }

    void VK_Buffer::release()
    {
#ifdef DEBUG_MODE
        Log::debug("Delete buffer " + VK_Utils::handleToString(handle));
#endif
        HB_ASSERT(allocated(), "VK_Buffer is not allocated and you are trying to release it");
        allocator->free(allocation);
        vkDestroyBuffer(device->getHandle(), handle, nullptr);
    }

    VK_Buffer::VK_Buffer(VK_Buffer&& other) noexcept
    {
        this->allocator = other.allocator;
        this->device = other.device;
        this->handle = other.handle;
        this->size = other.size;
        this->allocation = other.allocation;

        this->size = other.size;

        other.handle = VK_NULL_HANDLE;
        other.size = 0;
        other.allocation = {};
        other.device = nullptr;
        other.allocator = nullptr;
    }

    VK_Buffer& VK_Buffer::operator=(VK_Buffer&& other) noexcept
    {
        this->allocator = other.allocator;
        this->device = other.device;
        this->handle = other.handle;
        this->size = other.size;
        this->allocation = other.allocation;
        this->size = other.size;

        other.handle = VK_NULL_HANDLE;
        other.size = 0;
        other.allocation = {};
        other.device = nullptr;
        other.allocator = nullptr;
        return *this;
    }


    void VK_Buffer::copy(VK_Buffer* other)
    {
        allocator->copy(other->getHandle(), this->getHandle(), size);
    }

    VkDeviceSize VK_Buffer::getSize() const
    {
        return size;
    }

    const Allocation& VK_Buffer::getAllocation() const
    {
        return allocation;
    }

    void VK_Buffer::update(const void* data)
    {
        allocator->update(*this, data, size);
    }

    VkDeviceOrHostAddressConstKHR VK_Buffer::getDeviceAddress() const
    {
        VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
        bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAddressInfo.buffer = handle;

        VkDeviceOrHostAddressConstKHR buffer_address{};
        buffer_address.deviceAddress = device->vkGetBufferDeviceAddressKHR(
            device->getHandle(), &bufferDeviceAddressInfo);
        return buffer_address;
    }

    void VK_Buffer::map(void* data)
    {
        if (allocation.flags | MEMORY_TYPE_FLAG_MAPPABLE)
        {
            vkMapMemory(device->getHandle(), allocation.block->memory, allocation.offset, allocation.size, 0, &data);
        }
        else
        {
            Log::error("Buffer is not mappable");
        }
    }

    bool VK_Buffer::allocated()
    {
        return size > 0;
    }

    void VK_Buffer::reset()
    {
        allocation = {};
        handle = VK_NULL_HANDLE;
        size = 0;
    }
}
