#pragma once

#include "vulkan/vulkan.h"
#include "vector"
#include "list"
#include "unordered_map"
#include "set"
#include "string"
#include "queue"

namespace HBE {
	class VK_CommandPool;

	class VK_Buffer;

	class VK_Fence;

	class VK_Device;

	class VK_Image;

	class VK_Renderer;

	struct Block;
	enum ALLOC_FLAGS {
		ALLOC_FLAG_NONE = 0,
		ALLOC_FLAG_MAPPABLE = 1,
		ALLOC_FLAG_TEMP = 2,//
	};

	struct Allocation {
		VkDeviceSize size = 0;
		VkDeviceSize offset = 0;
		Block *block;
		ALLOC_FLAGS flags = ALLOC_FLAG_NONE;
		uint32_t id = 0;

		Allocation() {};

		Allocation(const Allocation &other) {
			this->size = other.size;
			this->offset = other.offset;
			this->block = other.block;
			this->flags = other.flags;
			this->id = other.id;
		}

		Allocation(VkDeviceSize size,
				   VkDeviceSize offset,
				   Block *block,
				   ALLOC_FLAGS flags,
				   uint32_t id) :
				size(size),
				offset(offset),
				block(block),
				flags(flags),
				id(id) {}

		bool operator<(const Allocation &other) const { return offset < other.offset; }
	};

	struct Block {
		VkDeviceSize size;
		std::set<Allocation> allocations = std::set<Allocation>();
		VkDeviceMemory memory = VK_NULL_HANDLE;
		uint32_t memory_type_index;
		uint32_t index;
		uint32_t alloc_count = 0;
		VkDeviceSize remaining;
	};

	struct StagingBuffer {
		Allocation allocation;
		VkBuffer buffer = VK_NULL_HANDLE;
		VK_Fence *fence;
	};

	class VK_Allocator {
	protected:
		const VkDeviceSize BLOCK_SIZE = 1024 * 1024 * 128; //1024*1024*128 = 128mb

		VK_CommandPool *command_pool;
		VK_Device *device;

		std::unordered_map<uint32_t, std::vector<Block *>> blocks;
		std::unordered_map<uint32_t, Block *> block_pool;
		const VkPhysicalDeviceMemoryProperties *memory_propeties;
		uint32_t current_id = 0;

		std::vector<Block> staging_blocks;

		std::queue<StagingBuffer> staging_buffers_queue;
	public:

		VK_Allocator(VK_Device *device);
		~VK_Allocator();
		virtual Allocation alloc(VkMemoryRequirements memory_requirement, ALLOC_FLAGS flags = ALLOC_FLAG_NONE);
		void free(const Allocation &allocation);
		void copy(VkBuffer src, VkBuffer dest, VkDeviceSize size);
		void copy(VkBuffer src, VK_Image *dest, VkImageLayout dst_end_layout);
		void copy(VK_Image *src, VkImageLayout src_end_layout, VK_Image *dest, VkImageLayout dst_end_layout);
		void setImageLayout(VK_Image *image, VkImageLayout newLayout);
		void update(const VK_Buffer &buffer, const void *data, size_t size);
		void update(VK_Image &image, const void *data, size_t width, size_t height);
	private:
		void barrierTransitionImageLayout(VK_Image *image, VkImageLayout new_layout);

		uint32_t findMemoryTypeIndex(VkMemoryRequirements memory_requirement, ALLOC_FLAGS flags);
		std::string memoryTypeToString(const uint32_t mem_type);
		std::string allocToString(const Allocation &alloc);

		void freeStagingBuffers();
		StagingBuffer createTempStagingBuffer(const void *data, size_t size);
	};
}

