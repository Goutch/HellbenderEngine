#pragma once

#include "stdint.h"
#include "vulkan/vulkan.h"
#include "data-structure/RawVector.h"
#include "VK_DescriptorPoolSize.h"
#include "vector"



namespace HBE {
	class VK_Context;
	class VK_PipelineLayout;

	struct DescriptorSetAllocation {
		uint32_t pool_alloc_index = 0;
		uint32_t descriptor_set_alloc_index = 0;
	};
	struct DescriptorPoolAllocation {
		VkDescriptorPool handle = VK_NULL_HANDLE;
		uint32_t max_sets = 0;
		VK_DescriptorPoolSize remaining_sizes;
		RawVector<DescriptorSetAllocation> allocations;
		RawVector<VkDescriptorSet> descriptor_sets;
	};

	class VK_DescriptorAllocator {
		VK_Context *context = nullptr;
		std::vector<DescriptorPoolAllocation> pools;
	public:
		void init(VK_Context *context);

		void release();

		VK_DescriptorAllocator(const VK_DescriptorAllocator &) = delete;

		VK_DescriptorAllocator &operator=(const VK_DescriptorAllocator &) = delete;

		void alloc(const VK_PipelineLayout *pipeline_layout, uint8_t *set_layout_indices, DescriptorSetAllocation *allocation_buffer, uint32_t count);

		void free(DescriptorSetAllocation allocation);

		VkDescriptorSet getDescriptorSet(DescriptorSetAllocation allocation);

		VK_DescriptorAllocator();

		uint32_t descriptorTypeToIndex(VkDescriptorType type);
	private:
		uint32_t findOrCreatePool(const VK_DescriptorPoolSize &required_pool_sizes);
	};
}
