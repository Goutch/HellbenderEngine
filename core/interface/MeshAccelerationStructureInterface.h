#pragma once
#include "MeshInterface.h"
#include "data-structure/Handle.h"
namespace HBE
{


    using MeshAccelerationStructureHandle = Handle;

    struct MeshAccelerationStructureInfo
    {
        MeshHandle mesh_handle;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
    };




}
