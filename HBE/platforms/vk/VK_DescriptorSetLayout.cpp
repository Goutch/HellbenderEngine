
#include "VK_DescriptorSetLayout.h"
#include "VK_Device.h"

namespace HBE {


	VK_DescriptorSetLayout::VK_DescriptorSetLayout(const VK_Device *device, uint32_t set, uint32_t binding, VkDeviceSize size, VkShaderStageFlags stage_flag) {
		this->device = device;
		this->set = set;
		this->binding = binding;
		this->size = size;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = binding;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = stage_flag;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &handle);
	}

	const VkDescriptorSetLayout &VK_DescriptorSetLayout::getHandle() const { return handle; }

	VkDeviceSize VK_DescriptorSetLayout::getSize() const { return size; }

	uint32_t VK_DescriptorSetLayout::getBinding() const { return binding; }

	uint32_t VK_DescriptorSetLayout::getSet() const { return set; }

	VK_DescriptorSetLayout::~VK_DescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(device->getHandle(), handle, nullptr);
	}

}