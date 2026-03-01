#pragma once
#include "Core.h"
#include "data-structure/Handle.h"

namespace HBE
{
    typedef uint32_t MEMORY_TYPE_FLAGS;

    enum MEMORY_TYPE_FLAG
    {
        MEMORY_TYPE_FLAG_NONE = 0,
        //The memory can be read/write by the cpu. Usually slower than gpu local memory.
        MEMORY_TYPE_FLAG_MAPPABLE = 1,
        //Faster memory type, but usually slower to send to the gpu since it need a staging buffer if it is not mappble.
        //There is sometime mappable and local memory but in low quantity (~256mb).
        MEMORY_TYPE_FLAG_GPU_LOCAL = 1 << 1,
    };

    class Context;
    using AllocatorHandle = Handle;

    struct AllocatorInfo
    {
    };

    class HB_API Allocator
    {
        AllocatorHandle handle;
        Context* context;

    public:
        Allocator(Context* context, AllocatorHandle handle);
    };
}
