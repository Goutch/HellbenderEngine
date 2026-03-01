#pragma once

#include "VK_PipelineLayout.h"
#include "core/resource/ComputePipeline.h"
#include "vulkan/vulkan.h"

namespace HBE
{
    class VK_Context;
    class VK_Renderer;

    class VK_PipelineLayout;

    class VK_ComputePipeline : public ComputePipeline
    {
        VkPipeline handle = VK_NULL_HANDLE;
        VK_PipelineLayout layout;
        ComputePipelineInfo info;
        VK_Context* context = nullptr;

    public:
        const vec3i& getWorkgroupSize() const override;
        COMPUTE_PIPELINE_FLAGS getFlags() const override;
        VK_ComputePipeline(VK_Context* context, const ComputePipelineInfo& info);
        ~VK_ComputePipeline();
        const VK_PipelineLayout& getPipelineLayout() const;
        VkPipeline getHandle() const;
    };
}
