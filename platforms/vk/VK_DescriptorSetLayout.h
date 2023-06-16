#pragma once

#include "vulkan/vulkan.h"
#include "vector"

namespace HBE {
	class VK_Device;

	struct VK_DescriptorInfo;

	class VK_DescriptorSetLayout {
		VK_Device *device = nullptr;
		VkDescriptorSetLayout handle = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		std::vector<VK_DescriptorInfo> descriptor_infos;
	public :
		VK_DescriptorSetLayout(VK_Device *device, uint32_t descriptor_set_index, std::vector<VK_DescriptorInfo> &pipeline_descriptors);

		~VK_DescriptorSetLayout();

		VkDescriptorSetLayout getHandle() const;

		const std::vector<VkDescriptorSetLayoutBinding> &getBindings() const;

		const std::vector<VK_DescriptorInfo> &getDescriptorInfos() const;
	};


}
