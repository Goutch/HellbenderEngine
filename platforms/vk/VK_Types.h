#pragma once
#include "core/interface/AllocatorInterface.h"

namespace HBE
{
    struct Block;

    struct Allocation
    {
        VkDeviceSize size = 0;
        VkDeviceSize offset = 0;
        Block* block = nullptr;
        MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE;
        uint32_t id = 0;
        uint32_t heap_index = 0;

        bool operator<(const Allocation& other) const { return offset < other.offset; }
    };
}
