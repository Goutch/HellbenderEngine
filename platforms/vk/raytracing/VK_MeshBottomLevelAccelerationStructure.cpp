#include "dependencies/utils-collection/Profiler.h"
#include "VK_MeshBottomLevelAccelerationStructure.h"
#include "../VK_Device.h"
#include "../VK_Buffer.h"
#include "../VK_Mesh.h"
#include "platforms/vk/VK_Context.h"

namespace HBE
{
    VK_MeshBottomLevelAccelerationStructure::VK_MeshBottomLevelAccelerationStructure(VK_Context* context, MeshAccelerationStructureInfo info)
    {
        this->context = context;
        HB_PROFILE_BEGIN("Build Mesh Acceleration Structure");

        //todo:initialize this
        VK_Mesh* mesh = dynamic_cast<VK_Mesh*>(info.mesh);
        VkDeviceSize vertex_size = mesh->getAttributeElementSize(0);

        VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
        accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;

        accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        accelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        accelerationStructureGeometry.geometry.triangles.vertexData = mesh->getBuffer(0).getDeviceAddress();
        accelerationStructureGeometry.geometry.triangles.maxVertex = mesh->getVertexCount();
        accelerationStructureGeometry.geometry.triangles.vertexStride = vertex_size;
        accelerationStructureGeometry.geometry.triangles.indexType = mesh->getIndicesSize() == sizeof(uint32_t) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
        accelerationStructureGeometry.geometry.triangles.indexData = mesh->getIndicesBuffer().getDeviceAddress();
        accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
        accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = nullptr;

        // Get size info
        VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
        accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        accelerationStructureBuildGeometryInfo.geometryCount = 1;
        accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

        const uint32_t numTriangles = mesh->hasIndexBuffer() ? mesh->getIndexCount() / 3 : mesh->getVertexCount() / 3;
        VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
        accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        context->device.vkGetAccelerationStructureBuildSizesKHR(
            context->device.getHandle(),
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &accelerationStructureBuildGeometryInfo,
            &numTriangles,
            &accelerationStructureBuildSizesInfo);

        BufferInfo buffer_info{};
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
        accelerationStructureBuildRangeInfo.primitiveCount = numTriangles;
        accelerationStructureBuildRangeInfo.primitiveOffset = 0;
        accelerationStructureBuildRangeInfo.firstVertex = 0;
        accelerationStructureBuildRangeInfo.transformOffset = 0;
        std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = {&accelerationStructureBuildRangeInfo};

        // Build the acceleration structure on the device via a one-time command buffer submission
        // Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds


        context->device.getQueue(QUEUE_FAMILY_GRAPHICS).beginCommand();
        context->device.vkCmdBuildAccelerationStructuresKHR(
            context->device.getQueue(QUEUE_FAMILY_GRAPHICS).getCommandPool()->getCurrentBuffer(),
            1,
            &accelerationBuildGeometryInfo,
            accelerationBuildStructureRangeInfos.data());
        context->device.getQueue(QUEUE_FAMILY_GRAPHICS).endCommand();
        context->device.getQueue(QUEUE_FAMILY_GRAPHICS).submitCommand().wait();
        scratchBuffer.release();
        VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
        accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        accelerationDeviceAddressInfo.accelerationStructure = handle;

        address.deviceAddress = context->device.vkGetAccelerationStructureDeviceAddressKHR(context->device.getHandle(), &accelerationDeviceAddressInfo);

        HB_PROFILE_END("Build Mesh Acceleration Structure");
    }

    VK_MeshBottomLevelAccelerationStructure::~VK_MeshBottomLevelAccelerationStructure()
    {
        buffer.release();
        context->device.vkDestroyAccelerationStructureKHR(context->device.getHandle(), handle, nullptr);
    }

    VkDeviceOrHostAddressConstKHR VK_MeshBottomLevelAccelerationStructure::getDeviceAddress() const
    {
        return address;
    }
}
