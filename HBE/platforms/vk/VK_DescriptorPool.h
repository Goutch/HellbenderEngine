#pragma once

#include <unordered_map>
#include "vulkan/vulkan.h"
#include "VK_CONSTANTS.h"
#include "VK_Buffer.h"
#include "array"

namespace HBE {
	class VK_Device;

	class VK_DescriptorSetLayout;

	class VK_DescriptorPool {
		VK_Device *device;
		VkDescriptorPool handle;
		std::array<std::vector<VkDescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptor_sets;
		std::unordered_map<uint32_t, std::vector<VK_Buffer *>> buffers;
		std::vector<const VK_DescriptorSetLayout *> layouts;
	public:
		~VK_DescriptorPool();
		VK_DescriptorPool(VK_Device *device, std::vector<const VK_DescriptorSetLayout *> &layouts);
		std::vector<VkDescriptorSet> &getSets(uint32_t i);
		std::vector<VK_Buffer *> &getBuffers(uint32_t binding);
	private:

		void createDescriptorSets();

	};
}

