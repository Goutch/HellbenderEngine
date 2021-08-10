//
// Created by User on 2021-08-01.
//

#include <vector>
#include "VK_DescriptorPool.h"
#include "VK_DescriptorSetLayout.h"
#include "VK_Device.h"

namespace HBE {
	VK_DescriptorPool::VK_DescriptorPool(VK_Device *device, std::vector<const VK_DescriptorSetLayout *> &layouts) {
		this->device = device;
		this->layouts = layouts;
		for (size_t i = 0; i < layouts.size(); ++i) {
			for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j) {
				buffers.emplace(layouts[i]->getBinding(),std::vector<VK_Buffer*>());
				buffers[layouts[i]->getBinding()].emplace_back( new VK_Buffer(device, layouts[i]->getSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_Buffer::MAPPABLE));
			}
		}
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = layouts.size();

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;

		poolInfo.maxSets = layouts.size()*MAX_FRAMES_IN_FLIGHT;

		if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create descriptor pool!");
		}
		createDescriptorSets();


	}

	void VK_DescriptorPool::createDescriptorSets() {
		VkDescriptorSetLayout *flat_layouts = new VkDescriptorSetLayout[layouts.size()];
		for (int i = 0; i < layouts.size(); ++i) {
			flat_layouts[i] = layouts[i]->getHandle();
		}
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = handle;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
			allocInfo.pSetLayouts = flat_layouts;
			descriptor_sets[i].resize(layouts.size());
			if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, descriptor_sets[i].data()) != VK_SUCCESS) {
				Log::error("failed to allocate descriptor sets!");
			}
			for (size_t j = 0; j < layouts.size(); ++j) {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = buffers[j][i]->getHandle();
				bufferInfo.offset = 0;
				bufferInfo.range = VK_WHOLE_SIZE;
				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptor_sets[i][j];
				descriptorWrite.dstBinding = layouts[j]->getBinding();
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;
				descriptorWrite.pImageInfo = nullptr; // Optional
				descriptorWrite.pTexelBufferView = nullptr; // Optional
				vkUpdateDescriptorSets(device->getHandle(), 1, &descriptorWrite, 0, nullptr);
			}
		}

		delete[] flat_layouts;
	}

	VK_DescriptorPool::~VK_DescriptorPool() {
		for (auto buffer_vector:buffers) {
			for (int i = 0; i < buffer_vector.second.size(); ++i) {
				delete buffer_vector.second[i];
			}
		}
		vkDestroyDescriptorPool(device->getHandle(), handle, nullptr);
	}


	std::vector<VkDescriptorSet> &VK_DescriptorPool::getSets(uint32_t i) {
		return descriptor_sets[i];
	}

	std::vector<VK_Buffer *> &VK_DescriptorPool::getBuffers(uint32_t binding) {
		return buffers[binding];
	}
}



