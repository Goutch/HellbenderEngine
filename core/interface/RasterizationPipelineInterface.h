#pragma once
#include "Handle.h"
#include "ShaderInterface.h"
namespace HBE {
    class Shader;

    class Image;

    class MeshLayout;

    class RasterizationTarget;

    typedef uint32_t RASTERIZATION_PIPELINE_FLAGS;

    enum RASTERIZATION_PIPELINE_FLAG {
        RASTERIZATION_PIPELINE_FLAG_NONE = 0,
        RASTERIZATION_PIPELINE_FLAG_CULL_BACK = 1 << 0,
        RASTERIZATION_PIPELINE_FLAG_CULL_FRONT = 1 << 1,
        RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST = 1 << 2,
        RASTERIZATION_PIPELINE_FLAG_FRONT_COUNTER_CLOCKWISE = 1 << 3,
        RASTERIZATION_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR = 1 << 4,
    };

    struct RasterizationPipelineInfo {
        ShaderHandle vertex_shader = HBE_NULL_HANDLE;
        ShaderHandle fragment_shader = HBE_NULL_HANDLE;
        ShaderHandle geometry_shader = HBE_NULL_HANDLE;
        const VertexAttributeInfo *attribute_infos = nullptr;
        RasterizationTarget *rasterization_target = nullptr;
        uint32_t attribute_info_count = 0;
        VERTEX_TOPOLOGY topology = VERTEX_TOPOLOGY_TRIANGLE_LIST;
        RASTERIZATION_PIPELINE_FLAGS flags = RASTERIZATION_PIPELINE_FLAG_NONE;
    };

    using RasterizationPipelineHandle = Handle;
}
