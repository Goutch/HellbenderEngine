#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Device;

	class VK_DescriptorSetLayout {
	private:
		VkDescriptorSetLayout handle;
		const VK_Device *device;
		VkDeviceSize size;
		uint32_t set;
		uint32_t binding;
	public:
		VK_DescriptorSetLayout(VK_DescriptorSetLayout &other) = delete;
		VK_DescriptorSetLayout(const VK_Device *device, uint32_t set, uint32_t binding, VkDeviceSize size, VkShaderStageFlags stage_flag);
		~VK_DescriptorSetLayout();
		const VkDescriptorSetLayout &getHandle() const;
		uint32_t getSet() const;
		uint32_t getBinding() const;
		VkDeviceSize getSize() const;
	};
}

