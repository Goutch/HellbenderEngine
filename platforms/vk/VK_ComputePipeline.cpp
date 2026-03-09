#include "VK_ComputePipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Shader.h"
#include "resources/VK_PipelineLayout.h"
#include "VK_Context.h"

namespace HBE
{
    VK_ComputePipeline::VK_ComputePipeline(VK_Context* context, const ComputePipelineInfo& info)
    {
        alloc(context, info);
    }

    VK_ComputePipeline::~VK_ComputePipeline()
    {
        release();
    }

    void VK_ComputePipeline::alloc(VK_Context* context, const ComputePipelineInfo& info)
    {
        this->context = context;
        const VK_Shader* vk_shader = context->pipelines.getShader(info.compute_shader);
        layout.init(context, &vk_shader, 1, info.flags & COMPUTE_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR);

        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stage.module = vk_shader->getHandle();
        stage.pName = "main";
        VkComputePipelineCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        create_info.layout = layout.getHandle();
        create_info.stage = stage;

        if (vkCreateComputePipelines(context->device.getHandle(), VK_NULL_HANDLE, 1, &create_info, nullptr,
                                     &handle) != VK_SUCCESS)
        {
            Log::error("Failed to create compute pipeline");
        }
    }

    void VK_ComputePipeline::release()
    {
        vkDestroyPipeline(context->device.getHandle(), handle, nullptr);
        layout.release();
        handle = VK_NULL_HANDLE;
    }


    const VK_PipelineLayout& VK_ComputePipeline::getPipelineLayout() const
    {
        return layout;
    }

    VkPipeline VK_ComputePipeline::getHandle() const
    {
        return handle;
    }

    bool VK_ComputePipeline::allocated()
    {
        return handle != VK_NULL_HANDLE;
    }
}
