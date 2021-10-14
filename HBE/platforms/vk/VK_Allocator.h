#pragma once

#include "vulkan/vulkan.h"
#include "vector"
#include "list"
#include "unordered_map"

namespace HBE {
	class VK_CommandPool;

	class VK_Buffer;

	class VK_Device;

	struct Block;
	enum AllocFlags {
		NONE = 0,
		MAPPABLE = 1,
	};
	struct Allocation {
		const VkDeviceSize size;
		const VkDeviceSize offset;
		Block &block;
		std::list<Allocation>::iterator position_in_block;
		AllocFlags flags;
	};
	struct Block {
		VkDeviceSize size;
		std::list<Allocation> allocations;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		uint32_t memory_type;
		uint32_t index;
		uint32_t alloc_count = 0;
		VkDeviceSize remaining;
	};

	class VK_Allocator {
		VK_CommandPool *command_pool;
		const VK_Device *device;
		const VkDeviceSize BLOCK_SIZE = 134217728; //1024*1024*128 = 128mb
		std::unordered_map<uint32_t, std::vector<Block>> blocks;
		const VkPhysicalDeviceMemoryProperties* memory_propeties;

	public:

		VK_Allocator(const VK_Device *device);

		~VK_Allocator();
		Allocation &alloc(VkMemoryRequirements memory_requirement, AllocFlags flags = NONE);

		void free(Allocation &allocation);
		void copy(VkBuffer src, VkBuffer dest, VkDeviceSize size);
		void copy(VkBuffer src, VkImage dest, uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1);
	private:
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		uint32_t findMemoryType(VkMemoryRequirements memory_requirement,AllocFlags flags);
		std::string memoryTypeToString(const uint32_t mem_type);
		std::string allocToString(const Allocation &alloc);
	};
}

