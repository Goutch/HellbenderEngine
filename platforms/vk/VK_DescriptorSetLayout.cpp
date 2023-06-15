#include "VK_DescriptorSetLayout.h"
#include "VK_Shader.h"
#include "VK_Device.h"

namespace HBE {

	VK_DescriptorSetLayout::VK_DescriptorSetLayout(VK_Device *device, uint32_t descriptor_set_index, const std::vector<VK_DescriptorInfo> &pipeline_descriptors) {
		this->device = device;
		for (const VK_DescriptorInfo &pipeline_descriptor: pipeline_descriptors) {
			if (pipeline_descriptor.descriptor_set == descriptor_set_index) {
				descriptor_infos.emplace_back(pipeline_descriptor);
			}
		}
		HB_ASSERT(descriptor_infos.empty(), "No descriptors for descriptor set " + std::to_string(descriptor_set_index));

		bool has_a_variable_size_binding = false;
		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flagsInfo{};
		std::vector<VkDescriptorBindingFlagsEXT> descriptor_binding_flags(pipeline_descriptors.size(), 0);
		for (int i = 0; i < descriptor_infos.size(); ++i) {
			if (descriptor_infos[i].variable_size) {
				has_a_variable_size_binding = true;
				for (int j = 0; j < pipeline_descriptors.size(); ++j) {
					descriptor_infos[i].layout_binding.descriptorCount -= pipeline_descriptors[j].layout_binding.descriptorCount;
				}
			}
			descriptor_binding_flags[i] = pipeline_descriptors[i].variable_size ? VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT : 0;
		}

		for (int i = 0; i < descriptor_infos.size(); ++i) {
			bindings.emplace_back(descriptor_infos[i].layout_binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();
		if (has_a_variable_size_binding) {
			layoutInfo.pNext = &flagsInfo;
			flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
			flagsInfo.bindingCount = bindings.size();
			flagsInfo.pBindingFlags = descriptor_binding_flags.data();
		}

		vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &handle);
	}

	VK_DescriptorSetLayout::~VK_DescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(device->getHandle(), handle, nullptr);
	}

	VkDescriptorSetLayout VK_DescriptorSetLayout::getHandle() const {
		return handle;
	}

	const std::vector<VkDescriptorSetLayoutBinding> &VK_DescriptorSetLayout::getBindings() const {
		return bindings;
	}

	const std::vector<VK_DescriptorInfo> &VK_DescriptorSetLayout::getDescriptorInfos() const {
		return descriptor_infos;
	}
}
