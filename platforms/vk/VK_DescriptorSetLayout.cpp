#include "VK_DescriptorSetLayout.h"

#include "VK_Context.h"
#include "HBE/platforms/vk/resources/VK_Shader.h"
#include "VK_Device.h"
#include "VK_PhysicalDevice.h"
#include "core/utility/Log.h"

namespace HBE {
	void VK_DescriptorSetLayout::init(VK_Context *context, uint32_t descriptor_set_index, std::vector<VK_BindingInfo> &pipeline_bindings, bool empty_descriptor_allowed) {
		this->context = context;
		bool variable_descriptor_reached = false;
		this->descriptor_set_index = descriptor_set_index;

		for (int i = 0; i < pipeline_bindings.size(); ++i) {
			if (pipeline_bindings[i].descriptor_set_index == descriptor_set_index) {
				set_bindings.emplace_back(pipeline_bindings[i]);

				HB_ASSERT(!variable_descriptor_reached, "Variable descriptor must only be the last descriptor in the descriptor set");
				if (pipeline_bindings[i].variable_size && !variable_descriptor_reached) {
					variable_descriptor_reached = true;
				}
				VkDescriptorPoolSize pool_size{};
				pool_size.type = pipeline_bindings[i].layout_binding.descriptorType;
				pool_size.descriptorCount = pipeline_bindings[i].layout_binding.descriptorCount;
				required_pool_sizes += (pool_size);
			}

		}

		HB_ASSERT(!set_bindings.empty(), "No descriptors for descriptor set " + std::to_string(descriptor_set_index));

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT flagsInfo{};
		std::vector<VkDescriptorBindingFlagsEXT> descriptor_binding_flags(set_bindings.size(), 0);

		VK_BindingInfo &last_descriptor = set_bindings[set_bindings.size() - 1];

		//this just make sure the same type descriptor do not exceed the max descriptor per stages not sure if needed
		//if (last_descriptor.variable_size)
		//{
		//    int same_type_variable_descriptors = 1;
		//    for (int i = 0; i < pipeline_descriptors.size(); ++i)
		//    {
		//        if (last_descriptor.layout_binding.descriptorType == pipeline_descriptors[i].layout_binding.descriptorType)
		//        {
		//            same_type_variable_descriptors++;
		//        }
		//    }
		//    for (int i = 0; i < pipeline_descriptors.size(); ++i)
		//    {
		//        if (last_descriptor.layout_binding.binding != pipeline_descriptors[i].layout_binding.binding &&
		//            pipeline_descriptors[i].descriptor_set_index == descriptor_set_index)
		//        {
		//            last_descriptor.layout_binding.descriptorCount -= pipeline_descriptors[i].layout_binding.descriptorCount;
		//        }
		//    }
		//    last_descriptor.layout_binding.descriptorCount = std::floor((float)last_descriptor.layout_binding.descriptorCount / (float)same_type_variable_descriptors);
		//}

		descriptor_binding_flags[descriptor_binding_flags.size() - 1] = last_descriptor.variable_size ? VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT : 0;

		if (empty_descriptor_allowed) {
			HB_ASSERT(context->physical_device.getEnabledExtensionFlags() & EXTENSION_FLAG_DESCRIPTOR_INDEXING, "Empty descriptor needs descriptor indexing extension  enabled");
			HB_ASSERT(context->physical_device.getDescriptorIndexingFeatures().descriptorBindingPartiallyBound, "Descriptor binding partially bound not supported by graphic card");
			for (int i = 0; i < descriptor_binding_flags.size(); ++i) {
				descriptor_binding_flags[i] |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
			}
		}
		for (int i = 0; i < set_bindings.size(); ++i) {
			layout_bindings.emplace_back(set_bindings[i].layout_binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = layout_bindings.size();
		layoutInfo.pBindings = layout_bindings.data();

		//binding flags require descriptor indexing enabled
		if (context->physical_device.getEnabledExtensionFlags() & EXTENSION_FLAG_DESCRIPTOR_INDEXING) {
			layoutInfo.pNext = &flagsInfo;
			flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
			flagsInfo.bindingCount = layout_bindings.size();
			flagsInfo.pBindingFlags = descriptor_binding_flags.data();
		}

		vkCreateDescriptorSetLayout(context->device.getHandle(), &layoutInfo, nullptr, &handle);


		for (int i = 0; i < set_bindings.size(); ++i) {
			pipeline_bindings[set_bindings[i].layout_binding.binding] = set_bindings[i];
		}
	}

	void VK_DescriptorSetLayout::release() {
		vkDestroyDescriptorSetLayout(context->device.getHandle(), handle, nullptr);
	}


	VK_DescriptorSetLayout::VK_DescriptorSetLayout(VK_DescriptorSetLayout &&other) noexcept {
		handle = other.handle;
		other.handle = 0;
		set_bindings = std::move(other.set_bindings);
		layout_bindings = std::move(other.layout_bindings);

		other.layout_bindings.clear();
		other.set_bindings.clear();
	}

	VkDescriptorSetLayout VK_DescriptorSetLayout::getHandle() const {
		return handle;
	}

	const std::vector<VkDescriptorSetLayoutBinding> &VK_DescriptorSetLayout::getLayoutBindings() const {
		return layout_bindings;
	}

	const std::vector<VK_BindingInfo> &VK_DescriptorSetLayout::getBindingInfos() const {
		return set_bindings;
	}

	uint32_t VK_DescriptorSetLayout::getLastBinding() const {
		return layout_bindings[layout_bindings.size() - 1].binding;
	}

	uint32_t VK_DescriptorSetLayout::getDescriptorSetIndex() const {
		return descriptor_set_index;
	}

	const VK_DescriptorPoolSize &VK_DescriptorSetLayout::getRequiredPoolSizes() const {
		return required_pool_sizes;
	}
}
