#pragma once

#include "vulkan/vulkan.h"
#include "vector"
#include "list"
#include "unordered_map"

namespace HBE {
	class VK_CommandPool;

	class VK_Buffer;

	class VK_Device;
	class VK_Image;
	struct Block;
	enum ALLOC_FLAGS {
		ALLOC_FLAG_NONE = 0,
		ALLOC_FLAG_MAPPABLE = 1,
	};
	struct Allocation {
		const VkDeviceSize size;
		const VkDeviceSize offset;
		Block &block;
		std::list<Allocation>::iterator position_in_block;
		ALLOC_FLAGS flags;
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
		Allocation &alloc(VkMemoryRequirements memory_requirement, ALLOC_FLAGS flags = ALLOC_FLAG_NONE);

		void free(Allocation &allocation);
		void copy(VkBuffer src, VkBuffer dest, VkDeviceSize size);
		void copy(VkBuffer src, VK_Image* dest,VkImageLayout dst_end_layout);
		void copy(VK_Image* src,VkImageLayout src_end_layout, VK_Image* dest,VkImageLayout dst_end_layout);
		void setImageLayout(VK_Image* image,VkImageLayout newLayout);
	private:
		void transitionImageLayout(VK_Image* image,VkImageLayout newLayout);

		uint32_t findMemoryType(VkMemoryRequirements memory_requirement, ALLOC_FLAGS flags);
		std::string memoryTypeToString(const uint32_t mem_type);
		std::string allocToString(const Allocation &alloc);
	};
}

