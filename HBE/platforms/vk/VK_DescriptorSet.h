#pragma once
#include "vulkan/vulkan.h"
namespace HBE{
	class VK_Device;
	class VK_DescriptorSet {
		VkDescriptorSet handle;
		VkDescriptorSetLayout layout;
		VK_Device* device;
	public:
		VK_DescriptorSet(VK_Device* device,VkDescriptorSetLayoutCreateInfo& createInfo);
		~VK_DescriptorSet();
		const VkDescriptorSetLayout & getLayout() const;
		VkDescriptorSet& getHandle();
	};

}
