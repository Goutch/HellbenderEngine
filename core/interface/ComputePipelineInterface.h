#pragma once
#include "Handle.h"
#include "ShaderInterface.h"

namespace HBE
{
    typedef uint32_t COMPUTE_PIPELINE_FLAG;

    enum COMPUTE_PIPELINE_FLAGS
    {
        COMPUTE_PIPELINE_FLAG_NONE = 0,
        COMPUTE_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR = 1,
    };

    struct ComputePipelineInfo
    {
        ShaderHandle compute_shader;
        COMPUTE_PIPELINE_FLAGS flags = COMPUTE_PIPELINE_FLAG_NONE;
    };
    using ComputePipelineHandle = Handle;
}
