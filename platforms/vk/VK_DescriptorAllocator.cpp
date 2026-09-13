#include "VK_DescriptorAllocator.h"
#include "core/utility/Log.h"
#include "VK_Context.h"

namespace HBE {
	VK_DescriptorAllocator::VK_DescriptorAllocator() {

	}

	void VK_DescriptorAllocator::init(VK_Context *context) {
		this->context = context;
	}

	void VK_DescriptorAllocator::release() {
		for (auto &pool: pools) {
			vkResetDescriptorPool(context->device.getHandle(), pool.handle, 0);
			vkDestroyDescriptorPool(context->device.getHandle(), pool.handle, nullptr);
		}
		pools.clear();
	}


	void VK_DescriptorAllocator::alloc(const VK_PipelineLayout *pipeline_layout, uint32_t *set_indices, DescriptorSetAllocation *allocation_buffer, uint32_t count) {
		HB_ASSERT(count <= 32, "Cannot allocate more than 32 descriptor sets at once (could easily be changed to a dynamic array if needed)");
		VkDescriptorSetLayout pipeline_layout_handles[32];
		VK_DescriptorPoolSize required_pool_sizes;
		for (int i = 0; i < count; ++i) {
			uint32_t set_index = set_indices[i];
			const VK_DescriptorPoolSize &set_required_pool_sizes = pipeline_layout->getDescriptorSetLayouts()[set_index].getRequiredPoolSizes();

			pipeline_layout_handles[i] = pipeline_layout->getDescriptorSetLayouts()[set_indices[i]].getHandle();
			required_pool_sizes += set_required_pool_sizes;
		}


		uint32_t pool_index = findOrCreatePool(required_pool_sizes);

		VkDescriptorSetAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.pSetLayouts = pipeline_layout_handles;
		alloc_info.descriptorSetCount = count;
		alloc_info.descriptorPool = pools[pool_index].handle;


		pools[pool_index].descriptor_sets.resize(pools[pool_index].descriptor_sets.size() + count);
		VkDescriptorSet *set_handles = pools[pool_index].descriptor_sets.end() - count;

		VkResult result = vkAllocateDescriptorSets(context->device.getHandle(), &alloc_info, set_handles);
		if (result != VK_SUCCESS) {
			Log::error("Failed to allocate descriptor set from pool");
		}

		//todo: update pool remaining sizes
	}


	void VK_DescriptorAllocator::free(DescriptorSetAllocation allocation) {
		DescriptorPoolAllocation &pool = pools[allocation.pool_alloc_index];
		vkFreeDescriptorSets(context->device.getHandle(), pool.handle, 1, &pool.descriptor_sets[allocation.descriptor_set_alloc_index]);
	}

	uint32_t VK_DescriptorAllocator::findOrCreatePool(const VK_DescriptorPoolSize &required_pool_sizes) {

		for (uint32_t i = 0; i < pools.size(); ++i) {
			DescriptorPoolAllocation &pool = pools[i];
			if (pool.remaining_sizes.fit(required_pool_sizes)) {
				return i;
			}
		}

		VkDescriptorPoolSize biggest_pool_size = required_pool_sizes.biggestSize();

		VK_DescriptorPoolSize pool_sizes;
		if (biggest_pool_size.descriptorCount < 64) {
			pool_sizes.init(64);
		} else if (biggest_pool_size.descriptorCount < 256) {
			pool_sizes.init(256);
		} else if (biggest_pool_size.descriptorCount < 1024) {
			pool_sizes.init(1024);
		} else {
			//custom pool for this alloc only
			pool_sizes = required_pool_sizes;
		}


		VkDescriptorPoolCreateInfo pool_create_info{};
		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		pool_create_info.pNext = nullptr,
		pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		pool_create_info.maxSets = 1024,
		pool_create_info.poolSizeCount = VK_DescriptorPoolSize::DESCRIPTOR_TYPES_COUNT,
		pool_create_info.pPoolSizes = pool_sizes.sizes;
		//No suitable pool found, alloc new pool here

		VkDescriptorPool pool_handle = VK_NULL_HANDLE;

		vkCreateDescriptorPool(context->device.getHandle(), &pool_create_info, nullptr, &pool_handle);

		pools.emplace_back(pool_handle, pool_create_info.maxSets);
		return pools.size() - 1;
	}

	VkDescriptorSet VK_DescriptorAllocator::getDescriptorSet(DescriptorSetAllocation allocation) {
		return pools[allocation.pool_alloc_index].descriptor_sets[allocation.descriptor_set_alloc_index];
	}


}