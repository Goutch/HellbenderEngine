#pragma once

#include "core/interface/AABBAccelerationStructureInterface.h"
#include "vulkan/vulkan.h"
#include "platforms/vk/VK_Buffer.h"

namespace HBE
{
    class VK_Context;
    class VK_Device;

    class Mesh;

    class VK_AABBBottomLevelAccelerationStructure
    {
        VkAccelerationStructureKHR handle;
        VK_Context* context = nullptr;
        VK_Buffer buffer{};
        VK_Buffer aabb_positions_buffer{};
        VkDeviceOrHostAddressConstKHR address;

    public:
        void alloc(VK_Context* context, AABBAccelerationStructureInfo info);
        void release();
        bool allocated();
        VK_AABBBottomLevelAccelerationStructure() = default;

        VkAccelerationStructureKHR getHandle() const;
        VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
        ~VK_AABBBottomLevelAccelerationStructure();
    };
}
