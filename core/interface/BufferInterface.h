#pragma once
#include "AllocatorInterface.h"
#include "Handle.h"
#include "ImageInterface.h"

namespace HBE
{
    typedef uint32_t TEXEL_BUFFER_FLAGS;

    enum TEXEL_BUFFER_FLAG
    {
        TEXEL_BUFFER_FLAG_NONE = 0,
    };

    struct TexelBufferInfo
    {
        IMAGE_FORMAT format = IMAGE_FORMAT_R8;
        uint32_t count = 0;
        TEXEL_BUFFER_FLAGS flags = TEXEL_BUFFER_FLAG_NONE;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
    };

    using TexelBufferHandle = Handle;

    typedef uint32_t STORAGE_BUFFER_FLAGS;

    enum STORAGE_BUFFER_FLAG
    {
        STORAGE_BUFFER_FLAG_NONE = 0,
    };

    struct StorageBufferInfo
    {
        uint32_t stride = 0;
        uint32_t count = 0;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
        STORAGE_BUFFER_FLAGS flags = STORAGE_BUFFER_FLAG_NONE;
    };

    using StorageBufferHandle = Handle;
}
