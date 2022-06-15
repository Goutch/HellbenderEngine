#pragma once

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "VK_PhysicalDevice.h"
#include "unordered_map"
#include "VK_Queue.h"

namespace HBE {
	class VK_RenderPass;

	class VK_Swapchain;

	class VK_GraphicPipeline;

	class VK_CommandPool;

	class VK_Allocator;

	class VK_Queue;

	class VK_Device {
		VkDevice handle;


		std::unordered_map<QUEUE_FAMILY, VK_Queue> queues;
		VK_PhysicalDevice *physical_device;
		VK_Allocator *allocator;


	public:
		VK_Device(VK_PhysicalDevice &physical_device);

		const VkDevice &getHandle() const;

		const VK_PhysicalDevice &getPhysicalDevice() const;


		VK_Queue &getQueue(QUEUE_FAMILY family);

		VK_Allocator *getAllocator();

		void wait();

		~VK_Device();


		//Extensions functions
		PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR = nullptr;
		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR = nullptr;
		PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = nullptr;
		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR = nullptr;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = nullptr;
		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR = nullptr;
		PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR = nullptr;
		PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR = nullptr;
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = nullptr;
		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = nullptr;

	private:
		void initFunctionPointers();
	};

}


