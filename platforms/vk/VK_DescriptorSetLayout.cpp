#include "VK_DescriptorSetLayout.h"
#include "VK_Shader.h"
#include "VK_Device.h"
#include "VK_PhysicalDevice.h"

namespace HBE {

	VK_DescriptorSetLayout::VK_DescriptorSetLayout(VK_Device *device, uint32_t descriptor_set_index, std::vector<VK_DescriptorInfo> &pipeline_descriptors, bool empty_descriptor_allowed) {
		this->device = device;
		bool variable_descriptor_reached = false;
		for (int i = 0; i < pipeline_descriptors.size(); ++i) {
			if (pipeline_descriptors[i].descriptor_set == descriptor_set_index) {
				descriptor_infos.emplace_back(pipeline_descriptors[i]);

				HB_ASSERT(!variable_descriptor_reached, "Variable descriptor must only be the last descriptor in the descriptor set");
				if (pipeline_descriptors[i].variable_size && !variable_descriptor_reached) {
					variable_descriptor_reached = true;
				}

			}
		}

		HB_ASSERT(!descriptor_infos.empty(), "No descriptors for descriptor set " + std::to_string(descriptor_set_index));

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flagsInfo{};
		std::vector<VkDescriptorBindingFlagsEXT> descriptor_binding_flags(descriptor_infos.size(), 0);

		VK_DescriptorInfo &last_descriptor = descriptor_infos[descriptor_infos.size() - 1];
		if (last_descriptor.variable_size) {
			for (int i = 0; i < pipeline_descriptors.size(); ++i) {
				if (last_descriptor.layout_binding.binding != pipeline_descriptors[i].layout_binding.binding) {
					last_descriptor.layout_binding.descriptorCount -= pipeline_descriptors[i].layout_binding.descriptorCount;
				}
			}
		}
		descriptor_binding_flags[descriptor_binding_flags.size() - 1] = last_descriptor.variable_size ?
		                                                                VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT : 0;
		
		if (empty_descriptor_allowed) {
			HB_ASSERT(device->getPhysicalDevice().getEnabledExtensionFlags() & EXTENSION_FLAG_DESCRIPTOR_INDEXING, "Empty descriptor needs descriptor indexing extension  enabled");
			HB_ASSERT(device->getPhysicalDevice().getDescriptorIndexingFeatures().descriptorBindingPartiallyBound, "Descriptor binding partially bound not supported by graphic card");
			for (int i = 0; i < descriptor_binding_flags.size(); ++i) {
				descriptor_binding_flags[i] |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
			}
		}
		for (int i = 0; i < descriptor_infos.size(); ++i) {
			bindings.emplace_back(descriptor_infos[i].layout_binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();

		//binding flags require descriptor indexing enabled
		if (device->getPhysicalDevice().getEnabledExtensionFlags() & EXTENSION_FLAG_DESCRIPTOR_INDEXING) {
			layoutInfo.pNext = &flagsInfo;
			flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
			flagsInfo.bindingCount = bindings.size();
			flagsInfo.pBindingFlags = descriptor_binding_flags.data();
		}

		vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &handle);


		for (int i = 0; i < descriptor_infos.size(); ++i) {
			pipeline_descriptors[descriptor_infos[i].layout_binding.binding] = descriptor_infos[i];
		}
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

	uint32_t VK_DescriptorSetLayout::getLastBinding() const {
		return bindings[bindings.size() - 1].binding;
	}

}
