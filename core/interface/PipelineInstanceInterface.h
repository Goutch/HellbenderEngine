#pragma once
namespace HBE
{
    typedef uint8_t PIPELINE_INSTANCE_FLAGS;

    enum PIPELINE_INSTANCE_FLAG : uint8_t
    {
        PIPELINE_INSTANCE_FLAG_NONE = 0,
    };

    struct UniformMemoryInfo
    {
        std::string name;
        uint32_t binding = 0; //used if name is empty or not found
        MEMORY_TYPE_FLAGS preferred_memory_type = MEMORY_TYPE_FLAG_GPU_LOCAL;
    };

    enum PIPELINE_INSTANCE_TYPE : uint8_t
    {
        PIPELINE_INSTANCE_TYPE_NONE = 0,
        PIPELINE_INSTANCE_TYPE_RASTERIZATION,
        PIPELINE_INSTANCE_TYPE_COMPUTE,
        PIPELINE_INSTANCE_TYPE_RAY_TRACING,
    };

    struct PipelineInstanceInfo
    {
        Handle pipeline_handle;
        PIPELINE_INSTANCE_TYPE type = PIPELINE_INSTANCE_TYPE_NONE;
        PIPELINE_INSTANCE_FLAGS flags = PIPELINE_INSTANCE_FLAG_NONE;
        MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
        UniformMemoryInfo* uniform_memory_type_infos; //optional, specify memory type for specific uniform buffers
        uint32_t uniform_memory_type_info_count = 0;
    };

    using PipelineInstanceHandle = Handle;
}
