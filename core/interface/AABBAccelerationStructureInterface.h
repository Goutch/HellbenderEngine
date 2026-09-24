#pragma once
#include "Handle.h"
#include "HBETypes.h"
#include "AllocatorInterface.h"
namespace HBE
{
    using AABBAccelerationStructureHandle = Handle;

    struct AABBAccelerationStructureInfo
    {
        vec3 min;
        vec3 max;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
    };
}
