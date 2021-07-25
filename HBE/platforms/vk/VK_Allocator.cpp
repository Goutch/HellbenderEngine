
#include "VK_Allocator.h"
#include "VK_CommandPool.h"
#include "VK_Buffer.h"
#include "VK_Device.h"

namespace HBE {
	VK_Allocator::VK_Allocator(const VK_Device *device) {
		this->device = device;
		this->command_pool = new VK_CommandPool(device, 1);
		vkGetPhysicalDeviceMemoryProperties(device->getPhysicalDevice().getHandle(), &memory_propeties);
	}

	VK_Allocator::~VK_Allocator() {
		delete command_pool;
		for (auto it = blocks.begin(); it != blocks.end(); ++it) {
			for (Block block:it->second) {
				vkFreeMemory(device->getHandle(), block.memory, nullptr);
			}
		}
	}

	uint32_t VK_Allocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		for (uint32_t i = 0; i < memory_propeties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memory_propeties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		Log::error("Failed to find suitable memory type");
		return 0;
	}

	//https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
	Allocation &VK_Allocator::alloc(VkBuffer &buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			Log::error("failed to create buffer!");
		}

		Allocation &allocation = getAllocation(buffer, size, properties);
		vkBindBufferMemory(device->getHandle(), buffer, allocation.block.memory, allocation.offset);
		return allocation;
	}

	void VK_Allocator::copy(VK_Buffer *src, VK_Buffer *dest) {
		command_pool->begin(0);
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // buffer offset not memory
		copyRegion.dstOffset = 0; // buffer offset not memory
		copyRegion.size = src->getSize();
		vkCmdCopyBuffer(command_pool->getCurrentBuffer(), src->getHandle(), dest->getHandle(), 1, &copyRegion);

		command_pool->end(0);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_pool->getCurrentBuffer();

		//todo use transfer queue
		vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device->getGraphicsQueue());
	}


	Allocation &VK_Allocator::getAllocation(VkBuffer &buffer, VkDeviceSize size, VkMemoryPropertyFlags properties) {
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device->getHandle(), buffer, &memRequirements);
		uint32_t memory_type = findMemoryType(memRequirements.memoryTypeBits, properties);
		if (blocks.find(memory_type) == blocks.end())
			blocks.emplace(memory_type, std::vector<Block>());
		for (auto block_it = blocks[memory_type].begin(); block_it != blocks[memory_type].end(); ++block_it) {
			VkDeviceSize position = 0;
			for (auto allocation_it = block_it->allocations.begin(); allocation_it != block_it->allocations.end(); ++allocation_it) {
				VkDeviceSize end = allocation_it->offset - (allocation_it->offset % memRequirements.alignment);
				VkDeviceSize available = (end - position);

				if (available >= size) {
					block_it->alloc_count++;
					Log::debug("memory_type=" + std::to_string(memory_type) + "|position=" + std::to_string(position) + "|alignment=" + std::to_string(memRequirements.alignment) + "|size=" + std::to_string(size));
					block_it->remaining -= size;
					std::list<Allocation>::iterator new_alloc_it = block_it->allocations.emplace(allocation_it, Allocation{
							.size=size,
							.offset=position,
							.block=*block_it});
					new_alloc_it->position_in_block = new_alloc_it;
					return *new_alloc_it;

				} else {
					position = allocation_it->offset + allocation_it->size;
					position += (memRequirements.alignment - (position % memRequirements.alignment));
				}
			}
			VkDeviceSize end = block_it->size - (block_it->size % memRequirements.alignment);
			if (end - position >= size) {
				block_it->alloc_count++;
				Log::debug("memory_type=" + std::to_string(memory_type) + "|position=" + std::to_string(position) + "|alignment=" + std::to_string(memRequirements.alignment) + "|size=" + std::to_string(size));
				block_it->remaining -= size;
				const std::list<Allocation>::iterator &new_alloc_it = block_it->allocations.emplace(block_it->allocations.end(),
																									Allocation{
																											.size=size,
																											.offset=position,
																											.block=*block_it});
				new_alloc_it->position_in_block = new_alloc_it;
				return *new_alloc_it;
			}
		}
		Log::debug("memory_type=" + std::to_string(memory_type) + "|new blocK!|alignment=" + std::to_string(memRequirements.alignment) + "|size=" + std::to_string(size));
		uint32_t index = blocks[memory_type].size();
		Block &block = blocks[memory_type].emplace_back(Block{
				.size= size > BLOCK_SIZE ? size : BLOCK_SIZE,
				.allocations=std::list<Allocation>(),
				.memory=VK_NULL_HANDLE,
				.memory_type=memory_type,
				.index=index,
				.alloc_count=0,
				.remaining=(size > BLOCK_SIZE ? size : BLOCK_SIZE) - size});


		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = block.size;
		allocInfo.memoryTypeIndex = memory_type;

		if (vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &block.memory) != VK_SUCCESS) {
			Log::error("failed to allocate buffer memory!");
		}
		block.alloc_count++;
		auto new_alloc_it = block.allocations.emplace(block.allocations.end(), Allocation{
				.size=size,
				.offset=0,
				.block=block});
		new_alloc_it->position_in_block = new_alloc_it;
		return *new_alloc_it;
	}

	void VK_Allocator::free(VkBuffer &buffer, Allocation &allocation) {

		vkDestroyBuffer(device->getHandle(), buffer, nullptr);
		allocation.block.alloc_count--;
		uint32_t memory_type = allocation.block.memory_type;

		Log::debug("free|memory_type=" +
				   std::to_string(memory_type) +
				   "|position=" + std::to_string(allocation.offset) +
				   "|size=" + std::to_string(allocation.size));

		if (allocation.block.alloc_count == 0) {
			vkFreeMemory(device->getHandle(), allocation.block.memory, nullptr);

			auto it = blocks[memory_type].erase(blocks[memory_type].begin() + allocation.block.index);

			for (; it != blocks[memory_type].end(); ++it) {
				it->index--;
			}
		}
		else
		{
			allocation.block.allocations.erase(allocation.position_in_block);
		}

	}
}
