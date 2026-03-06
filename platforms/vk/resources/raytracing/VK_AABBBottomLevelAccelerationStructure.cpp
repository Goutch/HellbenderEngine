#include "dependencies/utils-collection/Profiler.h"
#include "../VK_Device.h"
#include "VK_AABBBottomLevelAccelerationStructure.h"
#include "../VK_Buffer.h"
#include "vulkan/vulkan.h"
#include "../VK_CommandPool.h"
#include "platforms/vk/VK_Context.h"

namespace HBE
{
    VK_AABBBottomLevelAccelerationStructure::VK_AABBBottomLevelAccelerationStructure(VK_Context* context, AABBAccelerationStructureInfo info)
    {
        this->context = context;
        HB_PROFILE_BEGIN("Build AABB Acceleration Structure");


        VkAabbPositionsKHR aabb_positions{};
        aabb_positions.maxX = info.max.x;
        aabb_positions.maxY = info.max.y;
        aabb_positions.maxZ = info.max.z;
        aabb_positions.minX = info.min.x;
        aabb_positions.minY = info.min.y;
        aabb_positions.minZ = info.min.z;


        BufferInfo buffer_info{};
        buffer_info.data = static_cast<void*>(&aabb_positions);
        buffer_info.size = static_cast<VkDeviceSize>(sizeof(VkAabbPositionsKHR));
        buffer_info.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        buffer_info.memory_type_flags = info.preferred_memory_type_flags;
        aabb_positions_buffer.alloc(context, buffer_info);

        VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
        bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAddressInfo.buffer = aabb_positions_buffer.getHandle();

        VkDeviceOrHostAddressConstKHR aabb_position_buffer_address{};
        aabb_position_buffer_address.deviceAddress = context->device.vkGetBufferDeviceAddressKHR(context->device.getHandle(), &bufferDeviceAddressInfo);

        VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
        accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
        accelerationStructureGeometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
        accelerationStructureGeometry.geometry.aabbs.data = aabb_position_buffer_address;
        accelerationStructureGeometry.geometry.aabbs.stride = sizeof(VkAabbPositionsKHR);


        // Get size info
        VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
        accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        accelerationStructureBuildGeometryInfo.geometryCount = 1;
        accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;


        VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
        accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;


        context->device.vkGetAccelerationStructureBuildSizesKHR(
            context->device.getHandle(),
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &accelerationStructureBuildGeometryInfo,
            &accelerationStructureBuildGeometryInfo.geometryCount,
            &accelerationStructureBuildSizesInfo);
        buffer_info = {};
        buffer_info.data = nullptr;
        buffer_info.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
        buffer_info.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        buffer_info.memory_type_flags = info.preferred_memory_type_flags;
        buffer.alloc(context, buffer_info);


        VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
        accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        accelerationStructureCreateInfo.buffer = buffer.getHandle();
        accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
        accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        context->device.vkCreateAccelerationStructureKHR(context->device.getHandle(), &accelerationStructureCreateInfo, nullptr, &handle);

        // Create a small scratch buffer used during build of the bottom level acceleration structure
        buffer_info = {};
        buffer_info.size = accelerationStructureBuildSizesInfo.buildScratchSize;
        buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        buffer_info.memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
        VK_Buffer scratchBuffer{};
        scratchBuffer.alloc(context, buffer_info);

        VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
        accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        accelerationBuildGeometryInfo.dstAccelerationStructure = handle;
        accelerationBuildGeometryInfo.geometryCount = 1;
        accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
        accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.getDeviceAddress().deviceAddress;


        VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
        accelerationStructureBuildRangeInfo.primitiveCount = 1;
        std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = {&accelerationStructureBuildRangeInfo};
        VK_Device* device = &context->device;
        device->getQueue(QUEUE_FAMILY_GRAPHICS).beginCommand();
        device->vkCmdBuildAccelerationStructuresKHR(
            device->getQueue(QUEUE_FAMILY_GRAPHICS).getCommandPool()->getCurrentBuffer(),
            1,
            &accelerationBuildGeometryInfo,
            accelerationBuildStructureRangeInfos.data());
        device->getQueue(QUEUE_FAMILY_GRAPHICS).endCommand();
        device->getQueue(QUEUE_FAMILY_GRAPHICS).submitCommand().wait();

        scratchBuffer.release();
        VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
        accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        accelerationDeviceAddressInfo.accelerationStructure = handle;

        address.deviceAddress = device->vkGetAccelerationStructureDeviceAddressKHR(device->getHandle(), &accelerationDeviceAddressInfo);

        HB_PROFILE_END("Build AABB Acceleration Structure");
    }

    VK_AABBBottomLevelAccelerationStructure::~VK_AABBBottomLevelAccelerationStructure()
    {
        aabb_positions_buffer.release();
        buffer.release();
        context->device.vkDestroyAccelerationStructureKHR(context->device.getHandle(), handle, nullptr);
    }

    VkAccelerationStructureKHR VK_AABBBottomLevelAccelerationStructure::getHandle() const
    {
        return handle;
    }

    VkDeviceOrHostAddressConstKHR VK_AABBBottomLevelAccelerationStructure::getDeviceAddress() const
    {
        return address;
    }
}
