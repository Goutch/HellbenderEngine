#pragma once
#include "AllocatorInterface.h"
#include "Handle.h"
#include "ImageInterface.h"

namespace HBE {
    typedef uint32_t TEXEL_BUFFER_FLAGS;

    enum TEXEL_BUFFER_FLAG {
        TEXEL_BUFFER_FLAG_NONE = 0,
    };

    struct TexelBufferInfo {
        IMAGE_FORMAT format = IMAGE_FORMAT_R8;
        uint32_t count = 0;
        TEXEL_BUFFER_FLAGS flags = TEXEL_BUFFER_FLAG_NONE;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
    };

    using TexelBufferHandle = Handle;

    typedef uint32_t BUFFER_USAGE_FLAGS;

    enum BUFFER_USAGE_FLAG {
        BUFFER_USAGE_FLAG_NONE = 0,
        BUFFER_USAGE_FLAG_TRANSFER_SRC = 1,
        BUFFER_USAGE_FLAG_TRANSFER_DST = 2,
        BUFFER_USAGE_FLAG_UNIFORM = 4,
        BUFFER_USAGE_FLAG_VERTEX = 8,
        BUFFER_USAGE_FLAG_INDEX = 16,
        BUFFER_USAGE_FLAG_RAY_TRACING = 32,
        BUFFER_USAGE_FLAG_STORAGE_BUFFER = 64,
    };

    typedef uint32_t BUFFER_FLAGS;

    enum BUFFER_FLAG {
        BUFFER_FLAG_NONE = 0,
    };

    struct BufferInfo {
        uint32_t stride = 0;
        uint32_t count = 0;
        void *optional_data;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
        BUFFER_USAGE_FLAGS usage = BUFFER_USAGE_FLAG_NONE;
        BUFFER_FLAGS flags = BUFFER_FLAG_NONE;
    };

    using BufferHandle = Handle;
}
