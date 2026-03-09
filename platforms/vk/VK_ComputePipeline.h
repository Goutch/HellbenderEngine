#pragma once

#include "resources/VK_PipelineLayout.h"
#include "core/interface/ComputePipelineInterface.h"
#include "vulkan/vulkan.h"

namespace HBE
{
    class VK_Context;
    class VK_Renderer;

    class VK_PipelineLayout;

    class VK_ComputePipeline
    {
        VkPipeline handle = VK_NULL_HANDLE;
        VK_PipelineLayout layout;
        VK_Context* context = nullptr;

    public:
        const vec3i& getWorkgroupSize() const;
        COMPUTE_PIPELINE_FLAGS getFlags() const;
        bool allocated();
        VK_ComputePipeline() = default;
        VK_ComputePipeline(VK_Context* context, const ComputePipelineInfo& info);
        ~VK_ComputePipeline();
        void alloc(VK_Context* context, const ComputePipelineInfo& info);
        void release();
        const VK_PipelineLayout& getPipelineLayout() const;
        VkPipeline getHandle() const;
    };
}
