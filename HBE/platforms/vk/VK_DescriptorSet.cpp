#include "VK_DescriptorSet.h"
#include "VK_Shader.h"
#include "VK_Device.h"
namespace HBE{
	VK_DescriptorSet::VK_DescriptorSet(VK_Device* device,VkDescriptorSetLayoutCreateInfo& createInfo) {

	}
	const VkDescriptorSetLayout & VK_DescriptorSet::getLayout() const{
		return layout;
	}

	VkDescriptorSet &VK_DescriptorSet::getHandle() {
		return handle;
	}

	VK_DescriptorSet::~VK_DescriptorSet() {
		vkDestroyDescriptorSetLayout(device->getHandle(), layout, nullptr);
	}
}
