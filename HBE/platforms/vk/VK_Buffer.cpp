
#include "VK_Buffer.h"
#include "VK_Device.h"
#include "VK_Allocator.h"

namespace HBE {
    VK_Buffer::VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        this->device = device;
        this->allocation=&device->getAllocator().alloc(handle,size,usage,properties);
    }


    const VkBuffer &VK_Buffer::getHandle() const {
        return handle;
    }

    VK_Buffer::~VK_Buffer() {
        device->getAllocator().free(handle,*allocation);
    }

    void VK_Buffer::copy(VK_Buffer *other) {
        device->getAllocator().copy(other,this);
    }

    VkDeviceSize VK_Buffer::getSize() const {
        return allocation->size;
    }

    Allocation &VK_Buffer::getAllocation() {
        return *allocation;
    }
}