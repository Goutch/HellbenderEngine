#pragma once

#include "vulkan/vulkan.h"
#include "core/resource/raytracing/AccelerationStructure.h"
#include "platforms/vk/VK_Buffer.h"

namespace HBE
{
    class VK_Context;
    class VK_Device;

    class Mesh;

    class VK_AABBBottomLevelAccelerationStructure : public AABBAccelerationStructure
    {
        VkAccelerationStructureKHR handle;
        VK_Context* context = nullptr;
        VK_Buffer buffer{};
        VK_Buffer aabb_positions_buffer{};
        VkDeviceOrHostAddressConstKHR address;

    public:
        VK_AABBBottomLevelAccelerationStructure(VK_Context* context, AABBAccelerationStructureInfo info);

        VkAccelerationStructureKHR getHandle() const;
        VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
        ~VK_AABBBottomLevelAccelerationStructure();
    };
}
