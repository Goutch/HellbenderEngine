#pragma once

#include "vulkan/vulkan.h"
#include "vector"
#include "unordered_map"
#include "set"
#include "string"
#include "queue"
#include "VK_Mesh.h"
#include "core/resource/Image.h"
#include "core/graphics/Allocator.h"

namespace HBE {
    class VK_CommandPool;

    class VK_Buffer;

    class VK_Fence;

    class VK_Device;

    class VK_Image;

    class VK_Renderer;

    struct Block;

    struct Allocation {
        VkDeviceSize size = 0;
        VkDeviceSize offset = 0;
        Block *block;
        MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE;
        uint32_t id = 0;
        uint32_t heap_index = 0;

        Allocation() {
        };

        Allocation(const Allocation &other) {
            this->size = other.size;
            this->offset = other.offset;
            this->block = other.block;
            this->flags = other.flags;
            this->id = other.id;
            this->heap_index = other.heap_index;
        }

        Allocation(VkDeviceSize size,
                   VkDeviceSize offset,
                   Block *block,
                   MEMORY_TYPE_FLAGS flags,
                   uint32_t id,
                   uint32_t heap_index) : size(size),
                                          offset(offset),
                                          block(block),
                                          flags(flags),
                                          id(id),
                                          heap_index(heap_index) {
        }

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

    struct FreeRequest {
        Allocation allocation;
        VkBuffer vk_buffer = VK_NULL_HANDLE;
        VkImage vk_image = VK_NULL_HANDLE;
        VK_Image *image = nullptr;
        VK_Buffer *buffer = nullptr;
        StorageBuffer* storage_buffer = nullptr;
        VkFence fence = VK_NULL_HANDLE;
    };

    class VK_Allocator {
    protected:
        const VkDeviceSize BLOCK_SIZE = 1024 * 1024 * 128; // = 128mb

        VK_CommandPool *command_pool;
        VK_Device *device;

        struct MemoryHeapInfo {
            VkDeviceSize max_size;
            VkDeviceSize used_size;
            VkMemoryPropertyFlags memory_properties_flags;
            VkDeviceSize block_size;
            bool full;
        };

        struct MemoryTypeInfo {
            uint32_t index;
            uint32_t heap_index;
            uint32_t bit;
            VkMemoryPropertyFlags memory_properties_flags;
        };


        std::vector<MemoryTypeInfo> memory_types;
        std::vector<MemoryHeapInfo> memory_heaps;
        std::unordered_map<uint32_t, std::vector<Block *> > blocks;
        std::unordered_map<uint32_t, Block *> block_cache;
        const VkPhysicalDeviceMemoryProperties *memory_properties;
        uint32_t current_id = 0;

        std::vector<Block> staging_blocks;

        std::vector<FreeRequest> delete_queue;

    public:
        VK_Allocator(VK_Device *device);

        ~VK_Allocator();

        virtual Allocation alloc(VkMemoryRequirements memory_requirement,
                                 MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE);

        void free(const Allocation &allocation);

        VK_Fence *copy(VkBuffer src, VkBuffer dest, VkDeviceSize size, VkDeviceSize offset = 0);

        VK_Fence *copy(VkBuffer src, VK_Image *dest, VkImageLayout dst_end_layout, VkBufferImageCopy region);

        VK_Fence *copy(VK_Image *src, VkImageLayout src_end_layout, VK_Image *dest, VkImageLayout dst_end_layout);

        void setImageLayout(VK_Image *image, VkImageLayout newLayout);

        void update(const VK_Buffer &buffer, const void *data, size_t size, size_t offset = 0);

        void update(VK_Image &image, const void *data, uint32_t width, uint32_t depth, uint32_t height);

        void updateRegions(VK_Image &image, const void *data, uint32_t data_texel_count,
                           ImageRegionUpdateInfo *update_infos, uint32_t update_count);

        void processFreeRequests(uint32_t frame);

        void cmdBarrierTransitionImageLayout(VK_CommandPool *command_pool, VK_Image *image,
                                             VkImageLayout new_layout);

        VK_Fence *blitImage(VK_Image &src, VK_Image &dest);

        void freeLater(const FreeRequest &allocation);


    private:
        uint32_t findMemoryTypeIndex(VkMemoryRequirements memory_requirement, MEMORY_TYPE_FLAGS flags);

        std::string memoryTypeToString(const uint32_t mem_type);

        std::string allocToString(const Allocation &alloc);

        FreeRequest createTempStagingBuffer(const void *data, size_t size);

        void generateMipmaps(VK_Image &image);

    };
}
