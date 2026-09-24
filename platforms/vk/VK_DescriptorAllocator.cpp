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


	void VK_DescriptorAllocator::alloc(const VK_PipelineLayout *pipeline_layout,
									   uint32_t *set_layout_indices,
									   DescriptorSetAllocation *allocation_buffer,
									   uint32_t count,
									   uint32_t* variable_descriptor_counts) {
		RawVector<VkDescriptorSetLayout> pipeline_layout_handles;
		pipeline_layout_handles.resize(count);
		VK_DescriptorPoolSize required_pool_sizes;
		for (int i = 0; i < count; ++i) {
			uint32_t set_layout_index = set_layout_indices[i];
			const VK_DescriptorSetLayout &set_layout = pipeline_layout->getDescriptorSetLayouts()[set_layout_index];
			VK_DescriptorPoolSize set_required_pool_sizes = set_layout.getRequiredPoolSizes();

			if(variable_descriptor_counts != nullptr && variable_descriptor_counts[i] > 0) {
				VkDescriptorType variable_descriptor_type = set_layout.getLastBindingType();
				set_required_pool_sizes[variable_descriptor_type].descriptorCount += variable_descriptor_counts[i];
			}

			required_pool_sizes += set_required_pool_sizes;
			pipeline_layout_handles[i] = set_layout.getHandle();
		}

		uint32_t pool_index = findOrCreatePool(required_pool_sizes);

		VkDescriptorSetAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.pSetLayouts = pipeline_layout_handles.data();
		alloc_info.descriptorSetCount = count;
		alloc_info.descriptorPool = pools[pool_index].handle;
		VkDescriptorSetVariableDescriptorCountAllocateInfo variable_count_info{};
		if (variable_descriptor_counts!= nullptr) {
			bool descriptor_indexing_enabled = context->physical_device.getEnabledExtensionFlags() & EXTENSION_FLAG_DESCRIPTOR_INDEXING;
			HB_ASSERT((variable_descriptor_counts!= nullptr) == descriptor_indexing_enabled, "Descriptor indexing is not enabled but variable size descriptors are used!");
			variable_count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
			variable_count_info.descriptorSetCount = alloc_info.descriptorSetCount;
			variable_count_info.pDescriptorCounts = variable_descriptor_counts;
			for (int i = 0; i < count; ++i) {
				allocation_buffer[i].variable_size_descriptor_count = variable_descriptor_counts[i];
			}
			alloc_info.pNext = &variable_count_info;
		}
		pools[pool_index].descriptor_sets.resize(pools[pool_index].descriptor_sets.size() + count);
		VkDescriptorSet *set_handles = pools[pool_index].descriptor_sets.end() - count;

		VkResult result = vkAllocateDescriptorSets(context->device.getHandle(), &alloc_info, set_handles);
		if (result != VK_SUCCESS) {
			Log::error("Failed to allocate descriptor set from pool");
		}

		for (int i = 0; i < count; ++i) {
			allocation_buffer[i].pool_alloc_index = pool_index;
			allocation_buffer[i].descriptor_set_alloc_index = (pools[pool_index].descriptor_sets.size() - count) + i;
		}

		//recalculate total set sizes and store them into a cached allocation_sizes vector for the pool
		for (int i = 0; i < count; ++i) {
			const VK_DescriptorSetLayout& set_layout = pipeline_layout->getDescriptorSetLayouts()[set_layout_indices[i]];

			VK_DescriptorPoolSize set_required_pool_sizes = set_layout.getRequiredPoolSizes();
			set_required_pool_sizes[set_layout.getLastBindingType()].descriptorCount += allocation_buffer->variable_size_descriptor_count;

			pools[pool_index].allocation_sizes.add(set_required_pool_sizes);
		}

		pools[pool_index].remaining_sizes -= required_pool_sizes;
		pools[pool_index].allocations.addRange(allocation_buffer, count);
	}


	void VK_DescriptorAllocator::free(DescriptorSetAllocation allocation) {
		DescriptorPoolAllocation &pool = pools[allocation.pool_alloc_index];
		vkFreeDescriptorSets(context->device.getHandle(), pool.handle, 1, &pool.descriptor_sets[allocation.descriptor_set_alloc_index]);

		pool.remaining_sizes += pool.allocation_sizes[allocation.descriptor_set_alloc_index];
		pool.free_indices.add(allocation.descriptor_set_alloc_index);
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

		bool raytracing_enabled = context->physical_device.getEnabledExtensionFlags() & EXTENSION_FLAG::EXTENSION_FLAG_ACCELERATION_STRUCTURE;
		if (biggest_pool_size.descriptorCount < 64) {
			pool_sizes.initCore(64);
			if (raytracing_enabled) {
				pool_sizes.initRaytracing(64);
			}
		} else if (biggest_pool_size.descriptorCount < 256) {
			pool_sizes.initCore(256);
			if (raytracing_enabled) {
				pool_sizes.initRaytracing(256);
			}
		} else if (biggest_pool_size.descriptorCount < 1024) {
			pool_sizes.initCore(1024);
			if (raytracing_enabled) {
				pool_sizes.initRaytracing(1024);
			}
		} else {
			//custom pool for this alloc only
			pool_sizes = required_pool_sizes;
		}

		VkDescriptorPoolSize *descriptor_pool_sizes = pool_sizes.getNonZeroSizes();
		uint32_t descriptor_pool_sizes_count = pool_sizes.getNonZeroSizesCount();

		VkDescriptorPoolCreateInfo pool_create_info{};
		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		pool_create_info.pNext = nullptr,
		pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		pool_create_info.maxSets = 1024,
		pool_create_info.poolSizeCount = descriptor_pool_sizes_count;
		pool_create_info.pPoolSizes = descriptor_pool_sizes;

		VkDescriptorPool pool_handle = VK_NULL_HANDLE;

		vkCreateDescriptorPool(context->device.getHandle(), &pool_create_info, nullptr, &pool_handle);

		pools.emplace_back(pool_handle, pool_create_info.maxSets, pool_sizes);
		return pools.size() - 1;
	}

	VkDescriptorSet VK_DescriptorAllocator::getDescriptorSet(DescriptorSetAllocation allocation) {
		return pools[allocation.pool_alloc_index].descriptor_sets[allocation.descriptor_set_alloc_index];
	}

	void VK_DescriptorAllocator::copy(const VK_PipelineLayout* layout, uint32_t set_index, DescriptorSetAllocation &from, DescriptorSetAllocation to) {
		const VK_DescriptorSetLayout& set_layout = layout->getDescriptorSetLayouts()[set_index];
		std::vector<VkDescriptorSetLayoutBinding> bindings = set_layout.getLayoutBindings();
		std::vector<VkCopyDescriptorSet> copies;
		copies.reserve(bindings.size());
		for (int i = 0; i < bindings.size(); ++i) {

			VkCopyDescriptorSet copy_info{};
			copy_info.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			copy_info.srcSet = getDescriptorSet(from);
			copy_info.dstSet = getDescriptorSet(to);
			copy_info.srcBinding = bindings[i].binding;
			copy_info.dstBinding = bindings[i].binding;
			if(layout->IsBindingVariableSize(bindings[i].binding)) {
				copy_info.descriptorCount = from.variable_size_descriptor_count;

			} else {
				copy_info.descriptorCount = bindings[i].descriptorCount;
			}

			if(copy_info.descriptorCount==0) {
				continue;
			}

			copies.emplace_back(copy_info);
		}

		vkUpdateDescriptorSets(context->device.getHandle(), 0, nullptr, copies.size(), copies.data());
	}


}