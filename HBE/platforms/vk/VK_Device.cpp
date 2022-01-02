
#include "VK_Device.h"
#include "core/utility/Log.h"
#include "vector"
#include "set"
#include "VK_Allocator.h"
#include "VK_CONSTANTS.h"
namespace HBE {
	VK_Device::VK_Device(VK_PhysicalDevice &physical_device) {
		this->physical_device = &physical_device;
		QueueFamilyIndices indices = physical_device.getQueueFamilyIndices();

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphics_family.value(),
												  indices.present_family.value(),
												  indices.compute_family.value(),
												  indices.transfer_family.value()};

		float priority = 1.0f;//between 0 and 1.
		for (uint32_t queue_family:uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queue_create_info{};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &priority;
			queue_create_infos.push_back(queue_create_info);
		}

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		VkPhysicalDeviceFeatures device_features{};
		device_features.samplerAnisotropy = physical_device.getFeatures().samplerAnisotropy;
		device_create_info.pEnabledFeatures = &device_features;
		auto &enabled_extensions = physical_device.getRequiredExtensions();
		device_create_info.enabledExtensionCount = enabled_extensions.size();
		device_create_info.ppEnabledExtensionNames = enabled_extensions.data();

		if (vkCreateDevice(physical_device.getHandle(), &device_create_info, nullptr, &handle) != VK_SUCCESS) {
			Log::error("Failed to create logical device");
		}
        queues.emplace(QUEUE_FAMILY_GRAPHICS,new VK_Queue(this,indices.graphics_family.value()));
        queues.emplace(QUEUE_FAMILY_PRESENT,new VK_Queue(this,indices.present_family.value()));
        queues.emplace(QUEUE_FAMILY_COMPUTE,new VK_Queue(this,indices.compute_family.value()));
        queues.emplace(QUEUE_FAMILY_TRANSFER,new VK_Queue(this,indices.transfer_family.value()));
		/*vkGetDeviceQueue(handle, indices.graphics_family.value(), 0, &graphics_queue);
		vkGetDeviceQueue(handle, indices.present_family.value(), 0, &present_queue);
		vkGetDeviceQueue(handle, indices.compute_family.value(), 0, &compute_queue);
		vkGetDeviceQueue(handle, indices.transfer_family.value(), 0, &transfer_queue);*/

		allocator = new VK_Allocator(this);
	}

	VK_Device::~VK_Device() {
		delete allocator;
		vkDestroyDevice(handle, nullptr);

	}

	const VkDevice &VK_Device::getHandle() const {
		return handle;
	}



	const VK_PhysicalDevice &VK_Device::getPhysicalDevice() const {
		return *physical_device;
	}
   /* const VkQueue &VK_Device::getGraphicsQueue() const {
        return graphics_queue;
    }
	const VkQueue &VK_Device::getPresentQueue() const {
		return present_queue;
	}

    const VkQueue &VK_Device::getComputeQueue() const {
        return compute_queue;
    }

    const VkQueue &VK_Device::getTransferQueue() const {
        return transfer_queue;
    }*/
	void VK_Device::wait() {
		vkDeviceWaitIdle(handle);
	}


	VK_Allocator *VK_Device::getAllocator() {
		return allocator;
	}

    VK_Queue *VK_Device::getQueue(QUEUE_FAMILY family) {
        return queues[family];
    }

}