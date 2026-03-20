#include "VK_Buffer.h"
#include "HBE/platforms/vk/VK_Device.h"
#include "HBE/platforms/vk/VK_Context.h"
#include "HBE/platforms/vk/VK_Utils.h"

namespace HBE {
    VkBuffer VK_Buffer::getHandle() const {
        return handle;
    }

    bool VK_Buffer::allocated() {
        return size != 0;
    }

    void VK_Buffer::alloc(VK_Context *context, const BufferInfo &info) {
        VK_BufferInfo vk_buffer_info{};
        vk_buffer_info.preferred_memory_type_flag = info.preferred_memory_type_flags;
        vk_buffer_info.data = info.optional_data;
        vk_buffer_info.usage = 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_INDEX ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_VERTEX ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_STORAGE_BUFFER ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_TRANSFER_DST ? VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_TRANSFER_SRC ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_UNIFORM ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : 0;
        vk_buffer_info.usage |= info.usage & BUFFER_USAGE_FLAG_RAY_TRACING ? VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT : 0;
        alloc(context,vk_buffer_info);
    }

    void VK_Buffer::alloc(VK_Context *context, VK_BufferInfo &info) {
        this->device = &context->device;
        this->allocator = &context->allocator;

        if (info.size <= 0)
            return;
        this->size = info.size;
        std::vector<uint32_t> queues = {device->getQueue(QUEUE_FAMILY_GRAPHICS).getFamilyIndex()};

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = info.preferred_memory_type_flag & MEMORY_TYPE_FLAG_MAPPABLE ? info.usage : info.usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = queues.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = queues.size();
        bufferInfo.pQueueFamilyIndices = queues.data();

        if (vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create buffer!");
        }
#ifdef DEBUG_MODE
        Log::debug("Created buffer " + VK_Utils::handleToString(handle));
#endif
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(device->getHandle(), handle, &requirements);
        requirements.alignment = std::max(requirements.alignment, info.optional_custom_alignment);

        this->allocation = allocator->alloc(requirements, info.preferred_memory_type_flag);
        vkBindBufferMemory(device->getHandle(), handle, allocation.block->memory, allocation.offset);

        if (info.data != nullptr) {
            update(info.data);
        }
    }

    void VK_Buffer::release() {
#ifdef DEBUG_MODE
        Log::debug("Delete buffer " + VK_Utils::handleToString(handle));
#endif
        HB_ASSERT(allocated(), "VK_Buffer is not allocated and you are trying to release it");
        allocator->free(allocation);
        vkDestroyBuffer(device->getHandle(), handle, nullptr);
        size = 0;
    }

    VK_Buffer::VK_Buffer(VK_Buffer &&other) noexcept {
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

    VK_Buffer &VK_Buffer::operator=(VK_Buffer &&other) noexcept {
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


    void VK_Buffer::copy(VK_Buffer *other) {
        allocator->copy(other->getHandle(), this->getHandle(), size);
    }

    VkDeviceSize VK_Buffer::getSize() const {
        return size;
    }

    const Allocation &VK_Buffer::getAllocation() const {
        return allocation;
    }

    void VK_Buffer::update(const void *data) {
        //todo: add all copy commands to a single command buffer and submit once
        allocator->update(*this, data, size);
    }

    VkDeviceOrHostAddressConstKHR VK_Buffer::getDeviceAddress() const {
        VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
        bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAddressInfo.buffer = handle;

        VkDeviceOrHostAddressConstKHR buffer_address{};
        buffer_address.deviceAddress = device->vkGetBufferDeviceAddressKHR(
            device->getHandle(), &bufferDeviceAddressInfo);
        return buffer_address;
    }

    void VK_Buffer::map(void *data) {
        if (allocation.flags | MEMORY_TYPE_FLAG_MAPPABLE) {
            vkMapMemory(device->getHandle(), allocation.block->memory, allocation.offset, allocation.size, 0, &data);
        } else {
            Log::error("Buffer is not mappable");
        }
    }

    void VK_Buffer::reset() {
        allocation = {};
        handle = VK_NULL_HANDLE;
        size = 0;
    }
}
