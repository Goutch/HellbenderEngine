#pragma once
#include "core/resource/Allocator.h"

namespace HBE {
    struct Block;

    struct Allocation {
        VkDeviceSize size = 0;
        VkDeviceSize offset = 0;
        Block *block = nullptr;
        MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE;
        uint32_t id = 0;
        uint32_t heap_index = 0;
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
}
